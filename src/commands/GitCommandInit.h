#ifndef GITCOMMANDINIT_H
#define GITCOMMANDINIT_H

#include "GitCommand.h"

#include <QString>

/***************************************************************************/
/*                                                                         */
/*                           GitInitResult                                 */
/*                                                                         */
/***************************************************************************/

struct GitInitResult
{
    bool success = false;
    QString errorMessage;
    QString outputMessage;
};

/***************************************************************************/
/*                                                                         */
/*                           GitCommandInit                                */
/*                                                                         */
/***************************************************************************/

class GitCommandInit : public GitCommand
{
public:
    explicit GitCommandInit(const QString& repositoryPath = QString());

    GitInitResult execute() const;
};

#endif // GITCOMMANDINIT_H
