#ifndef GITCOMMANDCOMMIT_H
#define GITCOMMANDCOMMIT_H

#include "GitCommand.h"

#include <QString>

/***************************************************************************/
/*                                                                         */
/*                        GitCommitResult                                  */
/*                                                                         */
/***************************************************************************/

struct GitCommitResult
{
    bool success = false;
    QString errorMessage;
    QString commitHash;
};

/***************************************************************************/
/*                                                                         */
/*                        GitCommandCommit                                 */
/*                                                                         */
/***************************************************************************/

class GitCommandCommit : public GitCommand
{
public:
    explicit GitCommandCommit(const QString& repositoryPath = QString());

    void setMessage(const QString& message);
    QString message() const;

    GitCommitResult execute();

private:
    QString m_message;

    QString extractCommitHash(const QString& output) const;
};

#endif // GITCOMMANDCOMMIT_H
