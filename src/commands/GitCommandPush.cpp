#include "GitCommandPush.h"

#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                          GitCommandPush                                 */
/*                                                                         */
/***************************************************************************/

GitCommandPush::GitCommandPush(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommandPush::setRemote(const QString& remote)
{
    m_remote = remote;
}

QString GitCommandPush::remote() const
{
    return m_remote;
}

void GitCommandPush::setBranch(const QString& branch)
{
    m_branch = branch;
}

QString GitCommandPush::branch() const
{
    return m_branch;
}

GitPushResult GitCommandPush::execute() const
{
    GitPushResult result;

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
            << QStringLiteral("push")
            << trimmedRemote
            << trimmedBranch
        );

    result.success = processResult.success;
    result.outputMessage = processResult.outputMessage;
    result.errorMessage = processResult.errorMessage;

    return result;
}
