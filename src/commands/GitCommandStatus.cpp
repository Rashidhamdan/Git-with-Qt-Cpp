#include "GitCommandStatus.h"

#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                         GitCommandStatus                                */
/*                                                                         */
/***************************************************************************/

GitCommandStatus::GitCommandStatus(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommandStatus::setRepositoryPath(const QString& repositoryPath)
{
    GitCommand::setRepositoryPath(repositoryPath);
}

QString GitCommandStatus::repositoryPath() const
{
    return GitCommand::repositoryPath();
}

GitStatusResult GitCommandStatus::execute() const
{
    GitStatusResult result;

    if (m_repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    const GitProcessResult processResult = executeGitCommand(
        m_repositoryPath,
        QStringList()
            << QStringLiteral("status")
            << QStringLiteral("--porcelain")
        );

    if (!processResult.success) {
        result.errorMessage = processResult.errorMessage;
        return result;
    }

    result = parsePorcelainOutput(processResult.outputMessage);
    result.success = true;

    return result;
}

/***************************************************************************/
/*                                                                         */
/*                          Parsing Helpers                                */
/*                                                                         */
/***************************************************************************/

GitStatusResult GitCommandStatus::parsePorcelainOutput(const QString& output) const
{
    GitStatusResult result;
    result.success = true;

    const QStringList lines = output.split(QChar('\n'), Qt::SkipEmptyParts);

    for (QString line : lines) {
        if (line.endsWith(QChar('\r'))) {
            line.chop(1);
        }

        if (line.length() < 3) {
            continue;
        }

        const QChar indexStatus = line.at(0);
        const QChar workTreeStatus = line.at(1);

        QString path = line.mid(3).trimmed();
        path = normalizePath(path);

        if (path.isEmpty()) {
            continue;
        }

        if (indexStatus == QChar('?') && workTreeStatus == QChar('?')) {
            addUnique(result.untrackedFiles, path);
            continue;
        }

        if (indexStatus != QChar(' ')) {
            addUnique(result.stagedFiles, path);
        }

        if (workTreeStatus != QChar(' ')) {
            addUnique(result.modifiedFiles, path);
        }
    }

    return result;
}

void GitCommandStatus::addUnique(QStringList& list, const QString& value)
{
    const QString trimmedValue = value.trimmed();

    if (trimmedValue.isEmpty()) {
        return;
    }

    if (!list.contains(trimmedValue)) {
        list.append(trimmedValue);
    }
}

QString GitCommandStatus::normalizePath(const QString& rawPath)
{
    QString path = rawPath.trimmed();

    if (path.isEmpty()) {
        return QString();
    }

    if (path.startsWith(QChar('"')) && path.endsWith(QChar('"')) && path.length() >= 2) {
        path = path.mid(1, path.length() - 2);
    }

    const int renameSeparatorIndex = path.indexOf(QStringLiteral(" -> "));
    if (renameSeparatorIndex >= 0) {
        path = path.mid(renameSeparatorIndex + 4).trimmed();
    }

    path.replace(QChar('\\'), QChar('/'));

    return path;
}
