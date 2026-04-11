#ifndef GITCOMMANDSTATUS_H
#define GITCOMMANDSTATUS_H

#include "GitCommand.h"

#include <QString>
#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                         GitStatusResult                                 */
/*                                                                         */
/***************************************************************************/

struct GitStatusResult
{
    bool success = false;
    QString errorMessage;

    QStringList stagedFiles;
    QStringList modifiedFiles;
    QStringList untrackedFiles;
};

/***************************************************************************/
/*                                                                         */
/*                         GitCommandStatus                                */
/*                                                                         */
/***************************************************************************/

class GitCommandStatus : public GitCommand
{
public:
    explicit GitCommandStatus(const QString& repositoryPath = QString());

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    GitStatusResult execute() const;

private:
    GitStatusResult parsePorcelainOutput(const QString& output) const;
    static void addUnique(QStringList& list, const QString& value);
    static QString normalizePath(const QString& rawPath);
};

#endif // GITCOMMANDSTATUS_H
