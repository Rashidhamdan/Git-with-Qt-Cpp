#ifndef GITCOMMANDBRANCH_H
#define GITCOMMANDBRANCH_H

#include "GitCommand.h"

#include <QList>
#include <QString>

/***************************************************************************/
/*                                                                         */
/*                          GitBranchEntry                                 */
/*                                                                         */
/***************************************************************************/

struct GitBranchEntry
{
    QString name;
    QString fullName;
    bool isCurrent = false;
    bool isRemote = false;
};

/***************************************************************************/
/*                                                                         */
/*                          GitBranchResult                                */
/*                                                                         */
/***************************************************************************/

struct GitBranchResult
{
    bool success = false;
    QString errorMessage;
    QList<GitBranchEntry> localBranches;
    QList<GitBranchEntry> remoteBranches;
};

/***************************************************************************/
/*                                                                         */
/*                       GitBranchCreateResult                             */
/*                                                                         */
/***************************************************************************/

struct GitBranchCreateResult
{
    bool success = false;
    QString errorMessage;
    QString outputMessage;
};

/***************************************************************************/
/*                                                                         */
/*                         GitCommandBranch                                */
/*                                                                         */
/***************************************************************************/

class GitCommandBranch : public GitCommand
{
public:
    explicit GitCommandBranch(const QString& repositoryPath = QString());

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    GitBranchResult execute() const;
    GitBranchCreateResult createBranch(const QString& branchName,
                                       bool checkoutAfterCreate = false) const;

private:
    static QString normalizeBranchName(const QString& rawName);
    static bool isDetachedHeadLine(const QString& line);
};

#endif // GITCOMMANDBRANCH_H
