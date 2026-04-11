#include "GitCommandLog.h"

/***************************************************************************/
/*                                                                         */
/*                           GitCommandLog                                 */
/*                                                                         */
/***************************************************************************/

GitCommandLog::GitCommandLog(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommandLog::setRepositoryPath(const QString& repositoryPath)
{
    GitCommand::setRepositoryPath(repositoryPath);
}

QString GitCommandLog::repositoryPath() const
{
    return GitCommand::repositoryPath();
}

GitLogResult GitCommandLog::execute() const
{
    GitLogResult result;

    if (m_repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    const QString format =
        QStringLiteral("%h%x1f%H%x1f%an%x1f%ae%x1f%ad%x1f%s%x1f%d%x1e");

    const GitProcessResult processResult = executeGitCommand(
        m_repositoryPath,
        QStringList()
            << QStringLiteral("log")
            << QStringLiteral("--decorate")
            << QStringLiteral("--date=iso-strict")
            << QStringLiteral("--pretty=format:%1").arg(format)
            << QStringLiteral("-n")
            << QStringLiteral("200")
        );

    if (!processResult.success) {
        result.errorMessage = processResult.errorMessage;
        return result;
    }

    return parseLogOutput(processResult.outputMessage);
}

/***************************************************************************/
/*                                                                         */
/*                          Private Helpers                                */
/*                                                                         */
/***************************************************************************/

GitLogResult GitCommandLog::parseLogOutput(const QString& output) const
{
    GitLogResult result;
    result.success = true;

    const QChar fieldSeparator(0x1f);
    const QChar recordSeparator(0x1e);

    const QStringList records =
        output.split(recordSeparator, Qt::SkipEmptyParts);

    for (const QString& rawRecord : records) {
        const QString record = rawRecord.trimmed();
        if (record.isEmpty()) {
            continue;
        }

        const QStringList fields =
            record.split(fieldSeparator, Qt::KeepEmptyParts);

        if (fields.size() < 7) {
            continue;
        }

        GitLogEntry entry;
        entry.shortHash = fields.at(0).trimmed();
        entry.fullHash = fields.at(1).trimmed();
        entry.authorName = fields.at(2).trimmed();
        entry.authorEmail = fields.at(3).trimmed();
        entry.authorDate = fields.at(4).trimmed();
        entry.subject = fields.at(5).trimmed();
        entry.decorations = fields.at(6).trimmed();

        result.entries.append(entry);
    }

    return result;
}
