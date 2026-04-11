#include "GitCommand.h"

#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStandardPaths>

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

QString formatCommandText(const QStringList& arguments)
{
    return QStringLiteral("git %1").arg(arguments.join(QChar(' ')));
}
}

/***************************************************************************/
/*                                                                         */
/*                           GitCommand                                    */
/*                                                                         */
/***************************************************************************/

GitCommand::GitCommand(const QString& repositoryPath)
    : m_repositoryPath(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommand::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath;
}

QString GitCommand::repositoryPath() const
{
    return m_repositoryPath;
}

/***************************************************************************/
/*                                                                         */
/*                         Protected Helpers                               */
/*                                                                         */
/***************************************************************************/

QStringList GitCommand::gitSearchCandidates()
{
    QStringList candidates;

    appendUnique(candidates, QStandardPaths::findExecutable(QStringLiteral("git")));
    appendUnique(candidates, QStandardPaths::findExecutable(QStringLiteral("git.exe")));

#ifdef Q_OS_WIN
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

QString GitCommand::gitExecutablePath()
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

bool GitCommand::isGitAvailable(QString* resolvedGitPath, QString* errorMessage)
{
    const QString gitPath = gitExecutablePath();

    if (resolvedGitPath != nullptr) {
        *resolvedGitPath = gitPath;
    }

    if (errorMessage != nullptr) {
        errorMessage->clear();
    }

    if (!gitPath.trimmed().isEmpty()) {
        return true;
    }

    if (errorMessage != nullptr) {
#ifdef Q_OS_WIN
        *errorMessage =
            QStringLiteral("Git executable was not found.\n\n"
                           "Please install Git for Windows and make sure git.exe is available "
                           "in PATH or in a standard installation directory.");
#else
        *errorMessage =
            QStringLiteral("Git executable was not found.\n\n"
                           "Please install Git and make sure it is available in PATH.");
#endif
    }

    return false;
}

void GitCommand::configureGitProcess(QProcess& process,
                                     const QString& repositoryPath)
{
    QString resolvedGitPath;
    isGitAvailable(&resolvedGitPath);

    process.setWorkingDirectory(repositoryPath);
    process.setProgram(resolvedGitPath.isEmpty()
                           ? QStringLiteral("git")
                           : resolvedGitPath);
    process.setProcessChannelMode(QProcess::SeparateChannels);

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    prependPathEntries(environment, buildExtraGitPathEntries(resolvedGitPath));

#ifdef Q_OS_WIN
    environment.insert(QStringLiteral("TERM"), QStringLiteral("dumb"));
#endif

    process.setProcessEnvironment(environment);
}

QString GitCommand::buildStartFailureMessage(const QString& attemptedProgram,
                                             const QString& processError)
{
    QString message =
        QStringLiteral("Failed to start Git process.\n\nProgram: %1")
            .arg(attemptedProgram.trimmed().isEmpty()
                     ? QStringLiteral("git")
                     : attemptedProgram);

    if (!processError.trimmed().isEmpty()) {
        message += QStringLiteral("\nError: %1").arg(processError.trimmed());
    }

#ifdef Q_OS_WIN
    message += QStringLiteral(
        "\n\nMake sure Git for Windows is installed and git.exe is available.");
#else
    message += QStringLiteral(
        "\n\nMake sure Git is installed and available in PATH.");
#endif

    return message;
}

QString GitCommand::combineProcessOutput(const QString& standardOutput,
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

GitProcessResult GitCommand::executeGitCommand(const QString& repositoryPath,
                                               const QStringList& arguments,
                                               int timeoutMs)
{
    GitProcessResult result;

    if (repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    const QFileInfo repoInfo(repositoryPath);
    if (!repoInfo.exists() || !repoInfo.isDir()) {
        result.errorMessage =
            QStringLiteral("Repository path does not exist or is not a directory.");
        return result;
    }

    QString gitPath;
    if (!isGitAvailable(&gitPath, &result.errorMessage)) {
        return result;
    }

    QProcess process;
    configureGitProcess(process, repositoryPath);
    process.setArguments(arguments);
    process.start();

    if (!process.waitForStarted(timeoutMs)) {
        result.errorMessage = buildStartFailureMessage(process.program(),
                                                       process.errorString());
        return result;
    }

    if (!process.waitForFinished(timeoutMs)) {
        process.kill();
        process.waitForFinished();

        result.errorMessage =
            QStringLiteral("Git command timed out after %1 ms.\n\nCommand: %2")
                .arg(timeoutMs)
                .arg(formatCommandText(arguments));
        return result;
    }

    const QString standardOutput =
        QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    const QString standardError =
        QString::fromUtf8(process.readAllStandardError()).trimmed();

    result.exitCode = process.exitCode();
    result.exitStatus = process.exitStatus();
    result.outputMessage = combineProcessOutput(standardOutput, standardError);

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        result.errorMessage = !result.outputMessage.isEmpty()
        ? result.outputMessage
        : QStringLiteral("Git command failed.\n\nCommand: %1")
                .arg(formatCommandText(arguments));
        return result;
    }

    result.success = true;
    return result;
}
