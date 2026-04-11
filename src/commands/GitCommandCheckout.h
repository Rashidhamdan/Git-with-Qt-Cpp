#ifndef GITCOMMANDCHECKOUT_H
#define GITCOMMANDCHECKOUT_H

#include "GitCommand.h"

#include <QString>

/***************************************************************************/
/*                                                                         */
/*                        GitCheckoutResult                                */
/*                                                                         */
/***************************************************************************/

struct GitCheckoutResult
{
    bool success = false;
    QString errorMessage;
    QString outputMessage;
};

/***************************************************************************/
/*                                                                         */
/*                       GitCommandCheckout                                */
/*                                                                         */
/***************************************************************************/

class GitCommandCheckout : public GitCommand
{
public:
    explicit GitCommandCheckout(const QString& repositoryPath = QString());

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    void setBranchName(const QString& branchName);
    QString branchName() const;

    void setRemoteName(const QString& remoteName);
    QString remoteName() const;

    void setCreateTrackingBranch(bool createTrackingBranch);
    bool createTrackingBranch() const;

    GitCheckoutResult execute() const;

private:
    QString m_branchName;
    QString m_remoteName;
    bool m_createTrackingBranch = false;
};

#endif // GITCOMMANDCHECKOUT_H
