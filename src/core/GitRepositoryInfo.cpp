#include "GitRepositoryInfo.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                         Internal Helpers                                */
/*                                                                         */
/***************************************************************************/

namespace
{
void appendUnique(QStringList& list, const QString& value)
{
    const QString cleaned = QDir::fromNativeSeparators(value).trimmed();

    if (cleaned.isEmpty()) {
        return;
    }

    if (!list.contains(cleaned, Qt::CaseInsensitive)) {
        list.append(cleaned);
    }
}

void appendExistingDirectory(QStringList& list, const QString& path)
{
    const QString cleaned = QDir::fromNativeSeparators(path).trimmed();

    if (cleaned.isEmpty()) {
        return;
    }

    const QDir directory(cleaned);
    if (!directory.exists()) {
        return;
    }

    appendUnique(list, directory.absolutePath());
}

QStringList buildExtraGitPathEntries(const QString& gitExecutablePath)
{
    QStringList entries;

    if (gitExecutablePath.trimmed().isEmpty()) {
        return entries;
    }

    const QFileInfo gitInfo(gitExecutablePath);
    QDir gitDir = gitInfo.absoluteDir();

    appendExistingDirectory(entries, gitDir.absolutePath());

#ifdef Q_OS_WIN
    QDir gitRoot = gitDir;
    const QString dirName = gitDir.dirName().trimmed().toLower();

    if (dirName == QStringLiteral("cmd") || dirName == QStringLiteral("bin")) {
        gitRoot.cdUp();
    }

    appendExistingDirectory(entries, gitRoot.filePath(QStringLiteral("cmd")));
    appendExistingDirectory(entries, gitRoot.filePath(QStringLiteral("bin")));
    appendExistingDirectory(entries, gitRoot.filePath(QStringLiteral("usr/bin")));
    appendExistingDirectory(entries, gitRoot.filePath(QStringLiteral("mingw64/bin")));
    appendExistingDirectory(entries, gitRoot.filePath(QStringLiteral("mingw32/bin")));
#endif

    return entries;
}

void prependPathEntries(QProcessEnvironment& environment, const QStringList& newEntries)
{
    QStringList mergedEntries;

    for (const QString& entry : newEntries) {
        appendUnique(mergedEntries, entry);
    }

    const QString currentPath = environment.value(QStringLiteral("PATH"));
    const QStringList currentEntries =
        currentPath.split(QDir::listSeparator(), Qt::SkipEmptyParts);

    for (const QString& entry : currentEntries) {
        appendUnique(mergedEntries, entry);
    }

    QStringList nativeEntries;
    nativeEntries.reserve(mergedEntries.size());

    for (const QString& entry : mergedEntries) {
        nativeEntries.append(QDir::toNativeSeparators(entry));
    }

    environment.insert(QStringLiteral("PATH"),
                       nativeEntries.join(QDir::listSeparator()));
}

QString combineProcessOutput(const QString& standardOutput,
                             const QString& standardError)
{
    const QString trimmedOutput = standardOutput.trimmed();
    const QString trimmedError = standardError.trimmed();

    if (!trimmedOutput.isEmpty() && !trimmedError.isEmpty()) {
        return trimmedOutput + QStringLiteral("\n") + trimmedError;
    }

    if (!trimmedOutput.isEmpty()) {
        return trimmedOutput;
    }

    return trimmedError;
}
}

/***************************************************************************/
/*                                                                         */
/*                         Private Helpers                                 */
/*                                                                         */
/***************************************************************************/

QStringList GitRepositoryInfo::gitSearchCandidates()
{
    QStringList candidates;

    appendUnique(candidates, QStandardPaths::findExecutable(QStringLiteral("git")));
    appendUnique(candidates, QStandardPaths::findExecutable(QStringLiteral("git.exe")));

#ifdef Q_OS_WIN
    const QString applicationDirPath = QCoreApplication::applicationDirPath();
    if (!applicationDirPath.trimmed().isEmpty()) {
        const QDir appDir(applicationDirPath);
        appendUnique(candidates, appDir.filePath(QStringLiteral("git.exe")));
        appendUnique(candidates, appDir.filePath(QStringLiteral("tools/git.exe")));
        appendUnique(candidates, appDir.filePath(QStringLiteral("tools/git/cmd/git.exe")));
        appendUnique(candidates, appDir.filePath(QStringLiteral("tools/git/bin/git.exe")));
    }

    const QString programFiles = qEnvironmentVariable("ProgramFiles");
    const QString programFilesX86 = qEnvironmentVariable("ProgramFiles(x86)");
    const QString programW6432 = qEnvironmentVariable("ProgramW6432");
    const QString localAppData = qEnvironmentVariable("LocalAppData");

    const QStringList installRoots = {
        programFiles,
        programFilesX86,
        programW6432
    };

    for (const QString& root : installRoots) {
        if (root.trimmed().isEmpty()) {
            continue;
        }

        const QDir baseDir(root);
        appendUnique(candidates, baseDir.filePath(QStringLiteral("Git/cmd/git.exe")));
        appendUnique(candidates, baseDir.filePath(QStringLiteral("Git/bin/git.exe")));
    }

    if (!localAppData.trimmed().isEmpty()) {
        const QDir localDir(localAppData);
        appendUnique(candidates, localDir.filePath(QStringLiteral("Programs/Git/cmd/git.exe")));
        appendUnique(candidates, localDir.filePath(QStringLiteral("Programs/Git/bin/git.exe")));
    }

    appendUnique(candidates, QStringLiteral("C:/Program Files/Git/cmd/git.exe"));
    appendUnique(candidates, QStringLiteral("C:/Program Files/Git/bin/git.exe"));
    appendUnique(candidates, QStringLiteral("C:/Program Files (x86)/Git/cmd/git.exe"));
    appendUnique(candidates, QStringLiteral("C:/Program Files (x86)/Git/bin/git.exe"));
#else
    appendUnique(candidates, QStringLiteral("/usr/bin/git"));
    appendUnique(candidates, QStringLiteral("/usr/local/bin/git"));
    appendUnique(candidates, QStringLiteral("/bin/git"));
#endif

    return candidates;
}

QString GitRepositoryInfo::runGitCommand(const QString& repositoryPath,
                                         const QStringList& arguments,
                                         bool* success,
                                         QString* errorMessage,
                                         int timeoutMs)
{
    if (success != nullptr) {
        *success = false;
    }

    if (errorMessage != nullptr) {
        errorMessage->clear();
    }

    if (repositoryPath.trimmed().isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Repository path is empty.");
        }
        return QString();
    }

    const QFileInfo repositoryInfo(repositoryPath);
    if (!repositoryInfo.exists() || !repositoryInfo.isDir()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Repository path does not exist or is not a directory.");
        }
        return QString();
    }

    const QString gitExecutable = gitExecutablePath();
    if (gitExecutable.trimmed().isEmpty()) {
#ifdef Q_OS_WIN
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral(
                "Git executable was not found.\n\n"
                "Please install Git for Windows and make sure git.exe is available "
                "in PATH or in a standard installation directory."
                );
        }
#else
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral(
                "Git executable was not found.\n\n"
                "Please install Git and make sure it is available in PATH."
                );
        }
#endif
        return QString();
    }

    QProcess process;
    process.setWorkingDirectory(repositoryPath);
    process.setProgram(gitExecutable);
    process.setArguments(arguments);
    process.setProcessChannelMode(QProcess::SeparateChannels);

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    prependPathEntries(environment, buildExtraGitPathEntries(gitExecutable));

#ifdef Q_OS_WIN
    environment.insert(QStringLiteral("TERM"), QStringLiteral("dumb"));
    environment.insert(QStringLiteral("GIT_PAGER"), QStringLiteral("cat"));
    environment.insert(QStringLiteral("GIT_TERMINAL_PROMPT"), QStringLiteral("0"));
#endif

    process.setProcessEnvironment(environment);
    process.start();

    if (!process.waitForStarted(timeoutMs)) {
        if (errorMessage != nullptr) {
            QString message =
                QStringLiteral("Failed to start Git process.\n\nProgram: %1")
                    .arg(gitExecutable);

            if (!process.errorString().trimmed().isEmpty()) {
                message += QStringLiteral("\nError: %1").arg(process.errorString().trimmed());
            }

#ifdef Q_OS_WIN
            message += QStringLiteral(
                "\n\nMake sure Git for Windows is installed and git.exe is available."
                );
#else
            message += QStringLiteral(
                "\n\nMake sure Git is installed and available in PATH."
                );
#endif

            *errorMessage = message;
        }
        return QString();
    }

    if (!process.waitForFinished(timeoutMs)) {
        process.kill();
        process.waitForFinished();

        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Git process did not finish in time.");
        }
        return QString();
    }

    const QString standardOutput =
        QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    const QString standardError =
        QString::fromUtf8(process.readAllStandardError()).trimmed();

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (errorMessage != nullptr) {
            *errorMessage = !standardError.isEmpty()
            ? standardError
            : (!standardOutput.isEmpty()
                   ? standardOutput
                   : QStringLiteral("Git command failed."));
        }
        return QString();
    }

    if (success != nullptr) {
        *success = true;
    }

    return combineProcessOutput(standardOutput, standardError).trimmed();
}

/***************************************************************************/
/*                                                                         */
/*                          Public API                                     */
/*                                                                         */
/***************************************************************************/

bool GitRepositoryInfo::isGitAvailable()
{
    return !gitExecutablePath().trimmed().isEmpty();
}

QString GitRepositoryInfo::gitExecutablePath()
{
    const QStringList candidates = gitSearchCandidates();

    for (const QString& candidate : candidates) {
        if (candidate.trimmed().isEmpty()) {
            continue;
        }

        const QFileInfo info(candidate);

        if (!info.exists() || !info.isFile()) {
            continue;
        }

#ifdef Q_OS_WIN
        return QDir::toNativeSeparators(info.absoluteFilePath());
#else
        if (info.isExecutable()) {
            return QDir::toNativeSeparators(info.absoluteFilePath());
        }
#endif
    }

    return QString();
}

bool GitRepositoryInfo::isGitRepository(const QString& repositoryPath)
{
    bool success = false;

    const QString result = runGitCommand(
        repositoryPath,
        QStringList()
            << QStringLiteral("rev-parse")
            << QStringLiteral("--is-inside-work-tree"),
        &success
        );

    return success && result.compare(QStringLiteral("true"), Qt::CaseInsensitive) == 0;
}

QString GitRepositoryInfo::repositoryRoot(const QString& repositoryPath)
{
    bool success = false;

    const QString result = runGitCommand(
        repositoryPath,
        QStringList()
            << QStringLiteral("rev-parse")
            << QStringLiteral("--show-toplevel"),
        &success
        );

    if (!success) {
        return QString();
    }

    return QDir::fromNativeSeparators(result.trimmed());
}

QString GitRepositoryInfo::currentBranch(const QString& repositoryPath)
{
    bool success = false;

    const QString result = runGitCommand(
        repositoryPath,
        QStringList()
            << QStringLiteral("branch")
            << QStringLiteral("--show-current"),
        &success
        );

    if (!success || result.trimmed().isEmpty()) {
        return QString();
    }

    return result.trimmed();
}

QStringList GitRepositoryInfo::remotes(const QString& repositoryPath)
{
    bool success = false;

    const QString result = runGitCommand(
        repositoryPath,
        QStringList() << QStringLiteral("remote"),
        &success
        );

    if (!success || result.trimmed().isEmpty()) {
        return {};
    }

    QStringList remoteList = result.split(QChar('\n'), Qt::SkipEmptyParts);

    for (QString& remote : remoteList) {
        remote = remote.trimmed();
    }

    remoteList.removeAll(QString());
    remoteList.removeDuplicates();

    return remoteList;
}

QString GitRepositoryInfo::upstream(const QString& repositoryPath)
{
    bool success = false;

    const QString result = runGitCommand(
        repositoryPath,
        QStringList()
            << QStringLiteral("rev-parse")
            << QStringLiteral("--abbrev-ref")
            << QStringLiteral("--symbolic-full-name")
            << QStringLiteral("@{u}"),
        &success
        );

    if (!success || result.trimmed().isEmpty()) {
        return QString();
    }

    return result.trimmed();
}

bool GitRepositoryInfo::hasRemotes(const QString& repositoryPath)
{
    return !remotes(repositoryPath).isEmpty();
}

bool GitRepositoryInfo::hasUpstream(const QString& repositoryPath)
{
    return !upstream(repositoryPath).trimmed().isEmpty();
}
