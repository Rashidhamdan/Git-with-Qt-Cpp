#include "GitCommandPull.h"

#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                          GitCommandPull                                 */
/*                                                                         */
/***************************************************************************/

GitCommandPull::GitCommandPull(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommandPull::setRemote(const QString& remote)
{
    m_remote = remote;
}

QString GitCommandPull::remote() const
{
    return m_remote;
}

void GitCommandPull::setBranch(const QString& branch)
{
    m_branch = branch;
}

QString GitCommandPull::branch() const
{
    return m_branch;
}

GitPullResult GitCommandPull::execute() const
{
    GitPullResult result;

    if (m_repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    const QString trimmedRemote = m_remote.trimmed();
    if (trimmedRemote.isEmpty()) {
        result.errorMessage = QStringLiteral("Remote name is empty.");
        return result;
    }

    const QString trimmedBranch = m_branch.trimmed();
    if (trimmedBranch.isEmpty()) {
        result.errorMessage = QStringLiteral("Branch name is empty.");
        return result;
    }

    const GitProcessResult processResult = executeGitCommand(
        m_repositoryPath,
        QStringList()
            << QStringLiteral("pull")
            << QStringLiteral("--ff-only")
            << trimmedRemote
            << trimmedBranch
        );

    result.success = processResult.success;
    result.outputMessage = processResult.outputMessage;
    result.errorMessage = processResult.errorMessage;

    return result;
}
