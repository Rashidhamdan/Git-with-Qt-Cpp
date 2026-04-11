#include "GitCommandInit.h"

/***************************************************************************/
/*                                                                         */
/*                           GitCommandInit                                */
/*                                                                         */
/***************************************************************************/

GitCommandInit::GitCommandInit(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

GitInitResult GitCommandInit::execute() const
{
    GitInitResult result;

    if (m_repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    const GitProcessResult processResult = executeGitCommand(
        m_repositoryPath,
        QStringList() << QStringLiteral("init")
        );

    result.success = processResult.success;
    result.errorMessage = processResult.errorMessage;
    result.outputMessage = processResult.outputMessage;

    return result;
}
