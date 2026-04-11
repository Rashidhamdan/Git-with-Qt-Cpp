#ifndef GITCOMMANDLOG_H
#define GITCOMMANDLOG_H

#include "GitCommand.h"

#include <QList>
#include <QString>

/***************************************************************************/
/*                                                                         */
/*                           GitLogEntry                                   */
/*                                                                         */
/***************************************************************************/

struct GitLogEntry
{
    QString shortHash;
    QString fullHash;
    QString authorName;
    QString authorEmail;
    QString authorDate;
    QString subject;
    QString decorations;
};

/***************************************************************************/
/*                                                                         */
/*                           GitLogResult                                  */
/*                                                                         */
/***************************************************************************/

struct GitLogResult
{
    bool success = false;
    QString errorMessage;
    QList<GitLogEntry> entries;
};

/***************************************************************************/
/*                                                                         */
/*                           GitCommandLog                                 */
/*                                                                         */
/***************************************************************************/

class GitCommandLog : public GitCommand
{
public:
    explicit GitCommandLog(const QString& repositoryPath = QString());

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    GitLogResult execute() const;

private:
    GitLogResult parseLogOutput(const QString& output) const;
};

#endif // GITCOMMANDLOG_H
