#ifndef GITCOMMANDADD_H
#define GITCOMMANDADD_H

#include "GitCommand.h"

#include <QString>
#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                         GitAddResult                                    */
/*                                                                         */
/***************************************************************************/

struct GitAddResult
{
    bool success = false;
    QString errorMessage;
};

/***************************************************************************/
/*                                                                         */
/*                         GitCommandAdd                                   */
/*                                                                         */
/***************************************************************************/

class GitCommandAdd : public GitCommand
{
public:
    explicit GitCommandAdd(const QString& repositoryPath = QString());

    void setFiles(const QStringList& files);
    void setAddAll(bool addAll);

    GitAddResult execute();

private:
    QStringList m_files;
    bool m_addAll = false;

    QStringList buildArguments() const;
};

#endif // GITCOMMANDADD_H
