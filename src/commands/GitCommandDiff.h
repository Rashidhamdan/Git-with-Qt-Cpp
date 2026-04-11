#ifndef GITCOMMANDDIFF_H
#define GITCOMMANDDIFF_H

#include "GitCommand.h"

#include <QString>

/***************************************************************************/
/*                                                                         */
/*                           GitDiffResult                                 */
/*                                                                         */
/***************************************************************************/

struct GitDiffResult
{
    bool success = false;
    QString errorMessage;
    QString diffText;
};

/***************************************************************************/
/*                                                                         */
/*                          GitCommandDiff                                 */
/*                                                                         */
/***************************************************************************/

class GitCommandDiff : public GitCommand
{
public:
    explicit GitCommandDiff(const QString& repositoryPath = QString(),
                            const QString& filePath = QString());

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    void setFilePath(const QString& filePath);
    QString filePath() const;

    GitDiffResult execute() const;

private:
    QString m_filePath;
};

#endif // GITCOMMANDDIFF_H
