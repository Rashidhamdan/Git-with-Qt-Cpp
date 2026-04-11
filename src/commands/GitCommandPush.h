#ifndef GITCOMMANDPUSH_H
#define GITCOMMANDPUSH_H

#include "GitCommand.h"

#include <QString>

/***************************************************************************/
/*                                                                         */
/*                           GitPushResult                                 */
/*                                                                         */
/***************************************************************************/

struct GitPushResult
{
    bool success = false;
    QString errorMessage;
    QString outputMessage;
};

/***************************************************************************/
/*                                                                         */
/*                          GitCommandPush                                 */
/*                                                                         */
/***************************************************************************/

class GitCommandPush : public GitCommand
{
public:
    explicit GitCommandPush(const QString& repositoryPath = QString());

    void setRemote(const QString& remote);
    QString remote() const;

    void setBranch(const QString& branch);
    QString branch() const;

    GitPushResult execute() const;

private:
    QString m_remote;
    QString m_branch;
};

#endif // GITCOMMANDPUSH_H
