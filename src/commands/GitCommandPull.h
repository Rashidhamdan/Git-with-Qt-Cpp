#ifndef GITCOMMANDPULL_H
#define GITCOMMANDPULL_H

#include "GitCommand.h"

#include <QString>

/***************************************************************************/
/*                                                                         */
/*                           GitPullResult                                 */
/*                                                                         */
/***************************************************************************/

struct GitPullResult
{
    bool success = false;
    QString errorMessage;
    QString outputMessage;
};

/***************************************************************************/
/*                                                                         */
/*                          GitCommandPull                                 */
/*                                                                         */
/***************************************************************************/

class GitCommandPull : public GitCommand
{
public:
    explicit GitCommandPull(const QString& repositoryPath = QString());

    void setRemote(const QString& remote);
    QString remote() const;

    void setBranch(const QString& branch);
    QString branch() const;

    GitPullResult execute() const;

private:
    QString m_remote;
    QString m_branch;
};

#endif // GITCOMMANDPULL_H
