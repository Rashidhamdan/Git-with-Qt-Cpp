#include "CommandRouter.h"

#include "GitCommandAdd.h"
#include "GitCommandBranch.h"
#include "GitCommandCheckout.h"
#include "GitCommandCommit.h"
#include "GitCommandDiff.h"
#include "GitCommandLog.h"
#include "GitCommandPull.h"
#include "GitCommandPush.h"
#include "GitCommandStatus.h"

#include <QFileInfo>
#include <QProcess>
#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                         CommandRouter                                   */
/*                                                                         */
/***************************************************************************/

CommandRouter::CommandRouter(const QString& repositoryPath)
    : m_repositoryPath(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void CommandRouter::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath;
}

QString CommandRouter::repositoryPath() const
{
    return m_repositoryPath;
}

CommandRouteResult CommandRouter::route(const QString& input) const
{
    CommandParser parser;
    return route(parser.parse(input));
}

CommandRouteResult CommandRouter::route(const CommandParseResult& parseResult) const
{
    CommandRouteResult routeResult;

    if (!parseResult.success) {
        routeResult.errorMessage = parseResult.errorMessage.isEmpty()
        ? QStringLiteral("The command could not be parsed.")
        : parseResult.errorMessage;
        return routeResult;
    }

    const QString commandName = parseResult.commandName.trimmed().toLower();
    const QStringList arguments = parseResult.arguments;

    if (commandName == QStringLiteral("help")) {
        routeResult.success = true;
        routeResult.executedCommand = QStringLiteral("help");
        routeResult.outputMessage = buildHelpText();
        return routeResult;
    }

    if (commandName == QStringLiteral("status")) {
        if (!arguments.isEmpty()) {
            return runGitPassthrough(QStringList() << QStringLiteral("status") << arguments);
        }

        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        GitCommandStatus gitCommandStatus(m_repositoryPath);
        const GitStatusResult result = gitCommandStatus.execute();

        routeResult.executedCommand = QStringLiteral("status");

        if (!result.success) {
            routeResult.errorMessage = result.errorMessage.isEmpty()
            ? QStringLiteral("The repository status could not be loaded.")
            : result.errorMessage;
            return routeResult;
        }

        routeResult.success = true;
        routeResult.outputMessage = formatStatusOutput(result.stagedFiles,
                                                       result.modifiedFiles,
                                                       result.untrackedFiles);
        return routeResult;
    }

    if (commandName == QStringLiteral("log")) {
        if (!arguments.isEmpty()) {
            return runGitPassthrough(QStringList() << QStringLiteral("log") << arguments);
        }

        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        GitCommandLog gitCommandLog(m_repositoryPath);
        const GitLogResult result = gitCommandLog.execute();

        routeResult.executedCommand = QStringLiteral("log");

        if (!result.success) {
            routeResult.errorMessage = result.errorMessage.isEmpty()
            ? QStringLiteral("The commit history could not be loaded.")
            : result.errorMessage;
            return routeResult;
        }

        QStringList lines;

        if (result.entries.isEmpty()) {
            lines << QStringLiteral("No commits available.");
        } else {
            for (const GitLogEntry& entry : result.entries) {
                QString line = QStringLiteral("%1  %2")
                .arg(entry.shortHash, entry.subject);

                if (!entry.decorations.trimmed().isEmpty()) {
                    line += QStringLiteral("  %1").arg(entry.decorations.trimmed());
                }

                lines << line;
            }
        }

        routeResult.success = true;
        routeResult.outputMessage = formatLogOutput(lines);
        return routeResult;
    }

    if (commandName == QStringLiteral("diff")) {
        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        if (arguments.size() > 1 || (!arguments.isEmpty() && arguments.first().startsWith(QChar('-')))) {
            return runGitPassthrough(QStringList() << QStringLiteral("diff") << arguments);
        }

        GitCommandDiff gitCommandDiff(
            m_repositoryPath,
            arguments.isEmpty() ? QString() : arguments.first()
            );

        const GitDiffResult result = gitCommandDiff.execute();

        routeResult.executedCommand = QStringLiteral("diff");

        if (!result.success) {
            routeResult.errorMessage = result.errorMessage.isEmpty()
            ? QStringLiteral("The diff could not be loaded.")
            : result.errorMessage;
            return routeResult;
        }

        routeResult.success = true;
        routeResult.outputMessage = result.diffText.trimmed().isEmpty()
                                        ? QStringLiteral("No diff available.")
                                        : result.diffText;
        return routeResult;
    }

    if (commandName == QStringLiteral("add")) {
        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        if (arguments.isEmpty()) {
            routeResult.errorMessage = QStringLiteral("Usage: add --all | add <file1> [file2 ...]");
            return routeResult;
        }

        const bool addAll =
            (arguments.size() == 1 &&
             (arguments.first() == QStringLiteral("--all")
              || arguments.first() == QStringLiteral("-A")
              || arguments.first() == QStringLiteral(".")));

        for (const QString& arg : arguments) {
            if (!addAll && arg.startsWith(QChar('-'))) {
                return runGitPassthrough(QStringList() << QStringLiteral("add") << arguments);
            }
        }

        GitCommandAdd gitCommandAdd(m_repositoryPath);

        if (addAll) {
            gitCommandAdd.setAddAll(true);
            routeResult.executedCommand = QStringLiteral("add --all");
        } else {
            gitCommandAdd.setFiles(arguments);
            routeResult.executedCommand =
                QStringLiteral("add %1").arg(arguments.join(QStringLiteral(" ")));
        }

        const GitAddResult result = gitCommandAdd.execute();

        if (!result.success) {
            routeResult.errorMessage = result.errorMessage.isEmpty()
            ? QStringLiteral("The add operation failed.")
            : result.errorMessage;
            return routeResult;
        }

        routeResult.success = true;
        routeResult.repositoryStateChanged = true;
        routeResult.outputMessage = addAll
                                        ? QStringLiteral("All changes were staged successfully.")
                                        : QStringLiteral("%1 file(s) staged successfully.").arg(arguments.size());
        return routeResult;
    }

    if (commandName == QStringLiteral("commit")) {
        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        QString commitMessage;

        for (int i = 0; i < arguments.size(); ++i) {
            const QString arg = arguments.at(i);

            if (arg == QStringLiteral("-m") || arg == QStringLiteral("--message")) {
                if (i + 1 >= arguments.size()) {
                    routeResult.errorMessage = QStringLiteral("Usage: commit -m \"your message\"");
                    return routeResult;
                }

                commitMessage = arguments.mid(i + 1).join(QStringLiteral(" "));
                break;
            }
        }

        if (commitMessage.trimmed().isEmpty()) {
            return runGitPassthrough(QStringList() << QStringLiteral("commit") << arguments);
        }

        GitCommandCommit gitCommandCommit(m_repositoryPath);
        gitCommandCommit.setMessage(commitMessage);

        const GitCommitResult result = gitCommandCommit.execute();

        routeResult.executedCommand = QStringLiteral("commit -m %1").arg(commitMessage);

        if (!result.success) {
            routeResult.errorMessage = result.errorMessage.isEmpty()
            ? QStringLiteral("The commit operation failed.")
            : result.errorMessage;
            return routeResult;
        }

        routeResult.success = true;
        routeResult.repositoryStateChanged = true;
        routeResult.outputMessage = result.commitHash.trimmed().isEmpty()
                                        ? QStringLiteral("Commit created successfully.")
                                        : QStringLiteral("Commit created successfully.\nCommit: %1").arg(result.commitHash);
        return routeResult;
    }

    if (commandName == QStringLiteral("push")) {
        if (!arguments.isEmpty()) {
            return runGitPassthrough(QStringList() << QStringLiteral("push") << arguments);
        }

        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        GitCommandPush gitCommandPush(m_repositoryPath);
        const GitPushResult result = gitCommandPush.execute();

        routeResult.executedCommand = QStringLiteral("push");

        if (!result.success) {
            routeResult.errorMessage = result.errorMessage.isEmpty()
            ? QStringLiteral("The push operation failed.")
            : result.errorMessage;
            return routeResult;
        }

        routeResult.success = true;
        routeResult.repositoryStateChanged = true;
        routeResult.outputMessage = result.outputMessage.trimmed().isEmpty()
                                        ? QStringLiteral("Push completed successfully.")
                                        : result.outputMessage;
        return routeResult;
    }

    if (commandName == QStringLiteral("pull")) {
        if (!arguments.isEmpty()) {
            return runGitPassthrough(QStringList() << QStringLiteral("pull") << arguments);
        }

        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        GitCommandPull gitCommandPull(m_repositoryPath);
        const GitPullResult result = gitCommandPull.execute();

        routeResult.executedCommand = QStringLiteral("pull");

        if (!result.success) {
            routeResult.errorMessage = result.errorMessage.isEmpty()
            ? QStringLiteral("The pull operation failed.")
            : result.errorMessage;
            return routeResult;
        }

        routeResult.success = true;
        routeResult.repositoryStateChanged = true;
        routeResult.outputMessage = result.outputMessage.trimmed().isEmpty()
                                        ? QStringLiteral("Pull completed successfully.")
                                        : result.outputMessage;
        return routeResult;
    }

    if (commandName == QStringLiteral("branch")) {
        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        if (arguments.isEmpty() || arguments.first().compare(QStringLiteral("list"), Qt::CaseInsensitive) == 0) {
            GitCommandBranch gitCommandBranch(m_repositoryPath);
            const GitBranchResult result = gitCommandBranch.execute();

            routeResult.executedCommand = QStringLiteral("branch list");

            if (!result.success) {
                routeResult.errorMessage = result.errorMessage.isEmpty()
                ? QStringLiteral("The branch list could not be loaded.")
                : result.errorMessage;
                return routeResult;
            }

            QStringList localLines;
            QStringList remoteLines;

            for (const GitBranchEntry& entry : result.localBranches) {
                localLines << (entry.isCurrent
                                   ? QStringLiteral("* %1").arg(entry.name)
                                   : QStringLiteral("  %1").arg(entry.name));
            }

            for (const GitBranchEntry& entry : result.remoteBranches) {
                remoteLines << QStringLiteral("  %1").arg(entry.fullName);
            }

            routeResult.success = true;
            routeResult.outputMessage = formatBranchOutput(localLines, remoteLines);
            return routeResult;
        }

        if (arguments.first().compare(QStringLiteral("create"), Qt::CaseInsensitive) == 0) {
            if (arguments.size() < 2) {
                routeResult.errorMessage = QStringLiteral("Usage: branch create <branch-name>");
                return routeResult;
            }

            const QString branchName = arguments.at(1).trimmed();
            if (branchName.isEmpty()) {
                routeResult.errorMessage = QStringLiteral("Branch name is empty.");
                return routeResult;
            }

            GitCommandBranch gitCommandBranch(m_repositoryPath);
            const GitBranchCreateResult result = gitCommandBranch.createBranch(branchName, true);

            routeResult.executedCommand = QStringLiteral("branch create %1").arg(branchName);

            if (!result.success) {
                routeResult.errorMessage = result.errorMessage.isEmpty()
                ? QStringLiteral("The branch could not be created.")
                : result.errorMessage;
                return routeResult;
            }

            routeResult.success = true;
            routeResult.repositoryStateChanged = true;
            routeResult.outputMessage = result.outputMessage.trimmed().isEmpty()
                                            ? QStringLiteral("Created and checked out branch '%1'.").arg(branchName)
                                            : result.outputMessage;
            return routeResult;
        }

        return runGitPassthrough(QStringList() << QStringLiteral("branch") << arguments);
    }

    if (commandName == QStringLiteral("checkout")) {
        QString errorMessage;
        if (!isRepositoryPathUsable(&errorMessage)) {
            routeResult.errorMessage = errorMessage;
            return routeResult;
        }

        if (arguments.size() == 1 && !arguments.first().startsWith(QChar('-'))) {
            GitCommandCheckout gitCommandCheckout(m_repositoryPath);
            gitCommandCheckout.setBranchName(arguments.first().trimmed());
            gitCommandCheckout.setCreateTrackingBranch(false);

            const GitCheckoutResult result = gitCommandCheckout.execute();

            routeResult.executedCommand = QStringLiteral("checkout %1").arg(arguments.first());

            if (!result.success) {
                routeResult.errorMessage = result.errorMessage.isEmpty()
                ? QStringLiteral("The branch could not be checked out.")
                : result.errorMessage;
                return routeResult;
            }

            routeResult.success = true;
            routeResult.repositoryStateChanged = true;
            routeResult.outputMessage = result.outputMessage.trimmed().isEmpty()
                                            ? QStringLiteral("Switched branch successfully.")
                                            : result.outputMessage;
            return routeResult;
        }

        if (arguments.size() == 2 &&
            (arguments.first() == QStringLiteral("--track") || arguments.first() == QStringLiteral("-t"))) {
            const QString trackingRef = arguments.at(1).trimmed();
            const int slashIndex = trackingRef.indexOf(QChar('/'));

            if (slashIndex <= 0 || slashIndex >= trackingRef.length() - 1) {
                routeResult.errorMessage = QStringLiteral("Usage: checkout --track <remote>/<branch>");
                return routeResult;
            }

            const QString remoteName = trackingRef.left(slashIndex).trimmed();
            const QString branchName = trackingRef.mid(slashIndex + 1).trimmed();

            GitCommandCheckout gitCommandCheckout(m_repositoryPath);
            gitCommandCheckout.setRemoteName(remoteName);
            gitCommandCheckout.setBranchName(branchName);
            gitCommandCheckout.setCreateTrackingBranch(true);

            const GitCheckoutResult result = gitCommandCheckout.execute();

            routeResult.executedCommand =
                QStringLiteral("checkout --track %1/%2").arg(remoteName, branchName);

            if (!result.success) {
                routeResult.errorMessage = result.errorMessage.isEmpty()
                ? QStringLiteral("The tracking branch could not be checked out.")
                : result.errorMessage;
                return routeResult;
            }

            routeResult.success = true;
            routeResult.repositoryStateChanged = true;
            routeResult.outputMessage = result.outputMessage.trimmed().isEmpty()
                                            ? QStringLiteral("Tracking branch checked out successfully.")
                                            : result.outputMessage;
            return routeResult;
        }

        return runGitPassthrough(QStringList() << QStringLiteral("checkout") << arguments);
    }

    if (parseResult.explicitGitPassthrough) {
        if (arguments.isEmpty()) {
            routeResult.errorMessage = QStringLiteral("Usage: git <command> [arguments]");
            return routeResult;
        }

        return runGitPassthrough(arguments);
    }

    return runGitPassthrough(QStringList() << commandName << arguments);
}

/***************************************************************************/
/*                                                                         */
/*                         Repository Checks                               */
/*                                                                         */
/***************************************************************************/

bool CommandRouter::isRepositoryPathUsable(QString* errorMessage) const
{
    if (errorMessage != nullptr) {
        errorMessage->clear();
    }

    if (m_repositoryPath.trimmed().isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("No repository is currently set.");
        }
        return false;
    }

    const QFileInfo repoInfo(m_repositoryPath);
    if (!repoInfo.exists() || !repoInfo.isDir()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("The repository path does not exist or is not a directory.");
        }
        return false;
    }

    return true;
}

/***************************************************************************/
/*                                                                         */
/*                         Git Passthrough                                 */
/*                                                                         */
/***************************************************************************/

CommandRouteResult CommandRouter::runGitPassthrough(const QStringList& gitArguments) const
{
    CommandRouteResult routeResult;
    routeResult.usedPassthrough = true;

    if (gitArguments.isEmpty()) {
        routeResult.errorMessage = QStringLiteral("No git command was provided for passthrough.");
        return routeResult;
    }

    QString errorMessage;
    if (!isRepositoryPathUsable(&errorMessage)) {
        routeResult.errorMessage = errorMessage;
        return routeResult;
    }

    QProcess process;
    process.setWorkingDirectory(m_repositoryPath);
    process.start(QStringLiteral("git"), gitArguments);

    routeResult.executedCommand = QStringLiteral("git %1").arg(gitArguments.join(QStringLiteral(" ")));

    if (!process.waitForStarted()) {
        routeResult.errorMessage = QStringLiteral("Failed to start git passthrough process.");
        return routeResult;
    }

    if (!process.waitForFinished(-1)) {
        routeResult.errorMessage = QStringLiteral("git passthrough process did not finish correctly.");
        return routeResult;
    }

    const QString standardOutput = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    const QString standardError = QString::fromUtf8(process.readAllStandardError()).trimmed();

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        routeResult.errorMessage = !standardError.isEmpty()
        ? standardError
        : (!standardOutput.isEmpty()
               ? standardOutput
               : QStringLiteral("The git passthrough command failed."));
        return routeResult;
    }

    routeResult.success = true;
    routeResult.repositoryStateChanged = isStateChangingGitCommand(gitArguments.first().trimmed().toLower());
    routeResult.outputMessage = combineProcessOutput(standardOutput, standardError);

    if (routeResult.outputMessage.trimmed().isEmpty()) {
        routeResult.outputMessage = QStringLiteral("Command executed successfully.");
    }

    return routeResult;
}

QString CommandRouter::combineProcessOutput(const QString& standardOutput,
                                            const QString& standardError)
{
    if (!standardOutput.isEmpty() && !standardError.isEmpty()) {
        return standardOutput + QStringLiteral("\n") + standardError;
    }

    if (!standardOutput.isEmpty()) {
        return standardOutput;
    }

    return standardError;
}

bool CommandRouter::isStateChangingGitCommand(const QString& gitCommandName)
{
    const QString command = gitCommandName.trimmed().toLower();

    return command == QStringLiteral("add")
           || command == QStringLiteral("commit")
           || command == QStringLiteral("checkout")
           || command == QStringLiteral("switch")
           || command == QStringLiteral("branch")
           || command == QStringLiteral("merge")
           || command == QStringLiteral("rebase")
           || command == QStringLiteral("reset")
           || command == QStringLiteral("stash")
           || command == QStringLiteral("pull")
           || command == QStringLiteral("push")
           || command == QStringLiteral("fetch")
           || command == QStringLiteral("tag")
           || command == QStringLiteral("remote")
           || command == QStringLiteral("restore")
           || command == QStringLiteral("cherry-pick")
           || command == QStringLiteral("revert");
}

/***************************************************************************/
/*                                                                         */
/*                              Formatting                                 */
/*                                                                         */
/***************************************************************************/

QString CommandRouter::buildHelpText() const
{
    return QStringLiteral(
        "Hybrid CLI Help\n"
        "================\n"
        "\n"
        "Internally implemented commands:\n"
        "  help\n"
        "  status\n"
        "  log\n"
        "  diff [file]\n"
        "  add --all\n"
        "  add <file1> [file2 ...]\n"
        "  commit -m \"message\"\n"
        "  push\n"
        "  pull\n"
        "  branch list\n"
        "  branch create <branch-name>\n"
        "  checkout <branch-name>\n"
        "  checkout --track <remote>/<branch>\n"
        "\n"
        "Passthrough usage:\n"
        "  git <any git command>\n"
        "  <unknown command> [args]    -> automatically forwarded to git\n"
        "\n"
        "Console-only commands:\n"
        "  clear                       -> clears the console output\n"
        "\n"
        "Examples:\n"
        "  status\n"
        "  add --all\n"
        "  add src/gui/MainWindow.cpp\n"
        "  commit -m \"Fix branch manager\"\n"
        "  branch list\n"
        "  branch create feature/login\n"
        "  checkout main\n"
        "  checkout --track origin/feature/login\n"
        "  git stash\n"
        "  git remote -v\n"
        );
}

QString CommandRouter::formatStatusOutput(const QStringList& stagedFiles,
                                          const QStringList& modifiedFiles,
                                          const QStringList& untrackedFiles) const
{
    QString output;
    output += QStringLiteral("Repository Status\n");
    output += QStringLiteral("=================\n\n");

    auto appendSection = [&output](const QString& title, const QStringList& items) {
        output += title + QStringLiteral(":\n");

        if (items.isEmpty()) {
            output += QStringLiteral("  (none)\n\n");
            return;
        }

        for (const QString& item : items) {
            output += QStringLiteral("  - %1\n").arg(item);
        }

        output += QChar('\n');
    };

    appendSection(QStringLiteral("Staged Files"), stagedFiles);
    appendSection(QStringLiteral("Modified Files"), modifiedFiles);
    appendSection(QStringLiteral("Untracked Files"), untrackedFiles);

    if (stagedFiles.isEmpty() && modifiedFiles.isEmpty() && untrackedFiles.isEmpty()) {
        output += QStringLiteral("Working tree is clean.\n");
    }

    return output.trimmed();
}

QString CommandRouter::formatLogOutput(const QStringList& lines) const
{
    QString output;
    output += QStringLiteral("Commit History\n");
    output += QStringLiteral("==============\n\n");

    for (const QString& line : lines) {
        output += line + QChar('\n');
    }

    return output.trimmed();
}

QString CommandRouter::formatBranchOutput(const QStringList& localBranches,
                                          const QStringList& remoteBranches) const
{
    QString output;
    output += QStringLiteral("Branches\n");
    output += QStringLiteral("========\n\n");

    output += QStringLiteral("Local Branches:\n");
    if (localBranches.isEmpty()) {
        output += QStringLiteral("  (none)\n");
    } else {
        for (const QString& branch : localBranches) {
            output += branch + QChar('\n');
        }
    }

    output += QChar('\n');
    output += QStringLiteral("Remote Branches:\n");
    if (remoteBranches.isEmpty()) {
        output += QStringLiteral("  (none)\n");
    } else {
        for (const QString& branch : remoteBranches) {
            output += branch + QChar('\n');
        }
    }

    return output.trimmed();
}
