#include "GitCommandBranch.h"

#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                         Internal Helpers                                */
/*                                                                         */
/***************************************************************************/

namespace
{
QString combinedMessageOrFallback(const GitProcessResult& result,
                                  const QString& fallback)
{
    if (!result.outputMessage.trimmed().isEmpty()) {
        return result.outputMessage.trimmed();
    }

    if (!result.errorMessage.trimmed().isEmpty()) {
        return result.errorMessage.trimmed();
    }

    return fallback;
}
}

/***************************************************************************/
/*                                                                         */
/*                         GitCommandBranch                                */
/*                                                                         */
/***************************************************************************/

GitCommandBranch::GitCommandBranch(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

void GitCommandBranch::setRepositoryPath(const QString& repositoryPath)
{
    GitCommand::setRepositoryPath(repositoryPath);
}

QString GitCommandBranch::repositoryPath() const
{
    return GitCommand::repositoryPath();
}

GitBranchResult GitCommandBranch::execute() const
{
    GitBranchResult result;

    const GitProcessResult localResult = executeGitCommand(
        m_repositoryPath,
        QStringList()
            << QStringLiteral("branch")
            << QStringLiteral("--list")
            << QStringLiteral("--no-color")
        );

    if (!localResult.success) {
        result.errorMessage = localResult.errorMessage;
        return result;
    }

    const GitProcessResult remoteResult = executeGitCommand(
        m_repositoryPath,
        QStringList()
            << QStringLiteral("branch")
            << QStringLiteral("--remotes")
            << QStringLiteral("--no-color")
        );

    if (!remoteResult.success) {
        result.errorMessage = remoteResult.errorMessage;
        return result;
    }

    const QStringList localLines =
        localResult.outputMessage.split(QChar('\n'), Qt::SkipEmptyParts);

    for (const QString& rawLine : localLines) {
        const QString trimmedLine = rawLine.trimmed();
        if (trimmedLine.isEmpty()) {
            continue;
        }

        if (isDetachedHeadLine(trimmedLine)) {
            continue;
        }

        GitBranchEntry entry;
        entry.isRemote = false;
        entry.isCurrent = trimmedLine.startsWith(QStringLiteral("*"));

        QString branchText = trimmedLine;
        if (entry.isCurrent) {
            branchText = branchText.mid(1).trimmed();
        }

        entry.name = normalizeBranchName(branchText);
        entry.fullName = entry.name;

        if (!entry.name.isEmpty()) {
            result.localBranches.append(entry);
        }
    }

    const QStringList remoteLines =
        remoteResult.outputMessage.split(QChar('\n'), Qt::SkipEmptyParts);

    for (const QString& rawLine : remoteLines) {
        const QString trimmedLine = rawLine.trimmed();
        if (trimmedLine.isEmpty()) {
            continue;
        }

        if (trimmedLine.contains(QStringLiteral("->"))) {
            continue;
        }

        if (isDetachedHeadLine(trimmedLine)) {
            continue;
        }

        GitBranchEntry entry;
        entry.isRemote = true;
        entry.isCurrent = false;
        entry.fullName = normalizeBranchName(trimmedLine);

        const int slashIndex = entry.fullName.indexOf(QChar('/'));
        if (slashIndex >= 0 && slashIndex < entry.fullName.length() - 1) {
            entry.name = entry.fullName.mid(slashIndex + 1).trimmed();
        } else {
            entry.name = entry.fullName;
        }

        if (!entry.fullName.isEmpty()) {
            result.remoteBranches.append(entry);
        }
    }

    result.success = true;
    return result;
}

GitBranchCreateResult GitCommandBranch::createBranch(const QString& branchName,
                                                     bool checkoutAfterCreate) const
{
    GitBranchCreateResult result;

    const QString normalizedBranchName = normalizeBranchName(branchName);
    if (normalizedBranchName.isEmpty()) {
        result.errorMessage = QStringLiteral("Branch name is empty.");
        return result;
    }

    const GitProcessResult existsResult = executeGitCommand(
        m_repositoryPath,
        QStringList()
            << QStringLiteral("branch")
            << QStringLiteral("--list")
            << normalizedBranchName
        );

    if (!existsResult.success) {
        result.errorMessage = existsResult.errorMessage;
        return result;
    }

    if (!existsResult.outputMessage.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("A branch with this name already exists.");
        return result;
    }

    QStringList arguments;
    if (checkoutAfterCreate) {
        arguments
            << QStringLiteral("checkout")
            << QStringLiteral("-b")
            << normalizedBranchName;
    } else {
        arguments
            << QStringLiteral("branch")
            << normalizedBranchName;
    }

    const GitProcessResult createResult = executeGitCommand(
        m_repositoryPath,
        arguments
        );

    if (!createResult.success) {
        result.errorMessage = createResult.errorMessage;
        return result;
    }

    result.success = true;
    result.outputMessage = combinedMessageOrFallback(
        createResult,
        checkoutAfterCreate
            ? QStringLiteral("Branch \"%1\" was created and checked out.")
                  .arg(normalizedBranchName)
            : QStringLiteral("Branch \"%1\" was created.")
                  .arg(normalizedBranchName)
        );

    return result;
}

/***************************************************************************/
/*                                                                         */
/*                            Helpers                                      */
/*                                                                         */
/***************************************************************************/

QString GitCommandBranch::normalizeBranchName(const QString& rawName)
{
    QString name = rawName.trimmed();

    if (name.startsWith(QStringLiteral("*"))) {
        name = name.mid(1).trimmed();
    }

    return name;
}

bool GitCommandBranch::isDetachedHeadLine(const QString& line)
{
    const QString normalizedLine = line.trimmed();

    return normalizedLine.contains(QStringLiteral("HEAD detached"), Qt::CaseInsensitive)
           || normalizedLine.startsWith(QStringLiteral("(HEAD detached"), Qt::CaseInsensitive)
           || normalizedLine.startsWith(QStringLiteral("* (HEAD detached"), Qt::CaseInsensitive);
}
