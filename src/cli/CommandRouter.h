#ifndef COMMANDROUTER_H
#define COMMANDROUTER_H

#include <QString>
#include <QStringList>

#include "CommandParser.h"

/***************************************************************************/
/*                                                                         */
/*                       CommandRouteResult                                */
/*                                                                         */
/***************************************************************************/

struct CommandRouteResult
{
    bool success = false;
    bool usedPassthrough = false;
    bool repositoryStateChanged = false;

    QString executedCommand;
    QString outputMessage;
    QString errorMessage;
};

/***************************************************************************/
/*                                                                         */
/*                         CommandRouter                                   */
/*                                                                         */
/***************************************************************************/

class CommandRouter
{
public:
    explicit CommandRouter(const QString& repositoryPath = QString());

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    CommandRouteResult route(const QString& input) const;
    CommandRouteResult route(const CommandParseResult& parseResult) const;

private:
    QString m_repositoryPath;

private:
    bool isRepositoryPathUsable(QString* errorMessage = nullptr) const;

    CommandRouteResult runGitPassthrough(const QStringList& gitArguments) const;
    static QString combineProcessOutput(const QString& standardOutput,
                                        const QString& standardError);

    static bool isStateChangingGitCommand(const QString& gitCommandName);

    QString buildHelpText() const;
    QString formatStatusOutput(const QStringList& stagedFiles,
                               const QStringList& modifiedFiles,
                               const QStringList& untrackedFiles) const;
    QString formatLogOutput(const QStringList& lines) const;
    QString formatBranchOutput(const QStringList& localBranches,
                               const QStringList& remoteBranches) const;
};

#endif // COMMANDROUTER_H
