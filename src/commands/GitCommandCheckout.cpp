#include "GitCommandCheckout.h"

/***************************************************************************/
/*                                                                         */
/*                       GitCommandCheckout                                */
/*                                                                         */
/***************************************************************************/

GitCommandCheckout::GitCommandCheckout(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommandCheckout::setRepositoryPath(const QString& repositoryPath)
{
    GitCommand::setRepositoryPath(repositoryPath);
}

QString GitCommandCheckout::repositoryPath() const
{
    return GitCommand::repositoryPath();
}

void GitCommandCheckout::setBranchName(const QString& branchName)
{
    m_branchName = branchName;
}

QString GitCommandCheckout::branchName() const
{
    return m_branchName;
}

void GitCommandCheckout::setRemoteName(const QString& remoteName)
{
    m_remoteName = remoteName;
}

QString GitCommandCheckout::remoteName() const
{
    return m_remoteName;
}

void GitCommandCheckout::setCreateTrackingBranch(bool createTrackingBranch)
{
    m_createTrackingBranch = createTrackingBranch;
}

bool GitCommandCheckout::createTrackingBranch() const
{
    return m_createTrackingBranch;
}

GitCheckoutResult GitCommandCheckout::execute() const
{
    GitCheckoutResult result;

    if (m_repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    const QString trimmedBranchName = m_branchName.trimmed();
    if (trimmedBranchName.isEmpty()) {
        result.errorMessage = QStringLiteral("Branch name is empty.");
        return result;
    }

    QStringList arguments;
    arguments << QStringLiteral("checkout");

    if (m_createTrackingBranch) {
        const QString trimmedRemoteName = m_remoteName.trimmed();

        if (trimmedRemoteName.isEmpty()) {
            result.errorMessage = QStringLiteral("Remote name is empty.");
            return result;
        }

        arguments << QStringLiteral("-b")
                  << trimmedBranchName
                  << QStringLiteral("--track")
                  << QStringLiteral("%1/%2").arg(trimmedRemoteName, trimmedBranchName);
    } else {
        arguments << trimmedBranchName;
    }

    const GitProcessResult processResult =
        executeGitCommand(m_repositoryPath, arguments);

    result.success = processResult.success;
    result.outputMessage = processResult.outputMessage;
    result.errorMessage = processResult.errorMessage;

    return result;
}
