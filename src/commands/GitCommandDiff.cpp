#include "GitCommandDiff.h"

/***************************************************************************/
/*                                                                         */
/*                          GitCommandDiff                                 */
/*                                                                         */
/***************************************************************************/

GitCommandDiff::GitCommandDiff(const QString& repositoryPath,
                               const QString& filePath)
    : GitCommand(repositoryPath)
    , m_filePath(filePath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommandDiff::setRepositoryPath(const QString& repositoryPath)
{
    GitCommand::setRepositoryPath(repositoryPath);
}

QString GitCommandDiff::repositoryPath() const
{
    return GitCommand::repositoryPath();
}

void GitCommandDiff::setFilePath(const QString& filePath)
{
    m_filePath = filePath;
}

QString GitCommandDiff::filePath() const
{
    return m_filePath;
}

GitDiffResult GitCommandDiff::execute() const
{
    GitDiffResult result;

    if (m_repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    QStringList arguments;
    arguments << QStringLiteral("diff")
              << QStringLiteral("--no-color");

    if (!m_filePath.trimmed().isEmpty()) {
        arguments << QStringLiteral("--")
        << m_filePath.trimmed();
    }

    const GitProcessResult processResult =
        executeGitCommand(m_repositoryPath, arguments);

    result.success = processResult.success;
    result.errorMessage = processResult.errorMessage;
    result.diffText = processResult.outputMessage;

    return result;
}
