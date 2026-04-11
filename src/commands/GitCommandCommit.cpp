#include "GitCommandCommit.h"

#include <QRegularExpression>

/***************************************************************************/
/*                                                                         */
/*                        GitCommandCommit                                 */
/*                                                                         */
/***************************************************************************/

GitCommandCommit::GitCommandCommit(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommandCommit::setMessage(const QString& message)
{
    m_message = message;
}

QString GitCommandCommit::message() const
{
    return m_message;
}

GitCommitResult GitCommandCommit::execute()
{
    GitCommitResult result;

    if (m_repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    const QString trimmedMessage = m_message.trimmed();
    if (trimmedMessage.isEmpty()) {
        result.errorMessage = QStringLiteral("Commit message is empty.");
        return result;
    }

    const GitProcessResult processResult = executeGitCommand(
        m_repositoryPath,
        QStringList()
            << QStringLiteral("commit")
            << QStringLiteral("-m")
            << trimmedMessage
        );

    if (!processResult.success) {
        result.errorMessage = processResult.errorMessage;
        return result;
    }

    result.commitHash = extractCommitHash(processResult.outputMessage);

    if (result.commitHash.trimmed().isEmpty()) {
        const GitProcessResult hashResult = executeGitCommand(
            m_repositoryPath,
            QStringList()
                << QStringLiteral("rev-parse")
                << QStringLiteral("HEAD")
            );

        if (hashResult.success) {
            result.commitHash = hashResult.outputMessage.trimmed();
        }
    }

    result.success = true;
    return result;
}

/***************************************************************************/
/*                                                                         */
/*                          Private Helpers                                */
/*                                                                         */
/***************************************************************************/

QString GitCommandCommit::extractCommitHash(const QString& output) const
{
    static const QRegularExpression shortHashPattern(
        QStringLiteral("\\[[^\\]]+\\s+([0-9a-fA-F]{7,40})\\]")
        );

    static const QRegularExpression fullHashPattern(
        QStringLiteral("\\bcommit\\s+([0-9a-fA-F]{7,40})\\b")
        );

    QRegularExpressionMatch match = shortHashPattern.match(output);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }

    match = fullHashPattern.match(output);
    if (match.hasMatch()) {
        return match.captured(1).trimmed();
    }

    return QString();
}
