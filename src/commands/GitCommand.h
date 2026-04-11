#ifndef GITCOMMAND_H
#define GITCOMMAND_H

#include <QProcess>
#include <QString>
#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                         GitProcessResult                                */
/*                                                                         */
/***************************************************************************/

struct GitProcessResult
{
    bool success = false;
    QString outputMessage;
    QString errorMessage;
    int exitCode = -1;
    QProcess::ExitStatus exitStatus = QProcess::NormalExit;
};

/***************************************************************************/
/*                                                                         */
/*                             GitCommand                                  */
/*                                                                         */
/***************************************************************************/

class GitCommand
{
public:
    explicit GitCommand(const QString& repositoryPath = QString());
    virtual ~GitCommand() = default;

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

protected:
    static QString gitExecutablePath();
    static bool isGitAvailable(QString* resolvedGitPath = nullptr,
                               QString* errorMessage = nullptr);

    static void configureGitProcess(QProcess& process,
                                    const QString& repositoryPath);

    static GitProcessResult executeGitCommand(const QString& repositoryPath,
                                              const QStringList& arguments,
                                              int timeoutMs = 30000);

    static QStringList gitSearchCandidates();
    static QString buildStartFailureMessage(const QString& attemptedProgram,
                                            const QString& processError = QString());
    static QString combineProcessOutput(const QString& standardOutput,
                                        const QString& standardError);

protected:
    QString m_repositoryPath;
};

#endif // GITCOMMAND_H
