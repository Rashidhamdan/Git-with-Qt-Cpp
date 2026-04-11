#ifndef GITREPOSITORYINFO_H
#define GITREPOSITORYINFO_H

#include <QString>
#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                        GitRepositoryInfo                                */
/*                                                                         */
/***************************************************************************/

class GitRepositoryInfo
{
public:
    static bool isGitAvailable();
    static QString gitExecutablePath();

    static bool isGitRepository(const QString& repositoryPath);

    static QString repositoryRoot(const QString& repositoryPath);
    static QString currentBranch(const QString& repositoryPath);
    static QStringList remotes(const QString& repositoryPath);
    static QString upstream(const QString& repositoryPath);

    static bool hasRemotes(const QString& repositoryPath);
    static bool hasUpstream(const QString& repositoryPath);

private:
    static QString runGitCommand(const QString& repositoryPath,
                                 const QStringList& arguments,
                                 bool* success = nullptr,
                                 QString* errorMessage = nullptr,
                                 int timeoutMs = 10000);

    static QStringList gitSearchCandidates();
};

#endif // GITREPOSITORYINFO_H
