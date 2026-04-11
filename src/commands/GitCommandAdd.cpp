#include "GitCommandAdd.h"

/***************************************************************************/
/*                                                                         */
/*                         GitCommandAdd                                   */
/*                                                                         */
/***************************************************************************/

GitCommandAdd::GitCommandAdd(const QString& repositoryPath)
    : GitCommand(repositoryPath)
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void GitCommandAdd::setFiles(const QStringList& files)
{
    m_files = files;
}

void GitCommandAdd::setAddAll(bool addAll)
{
    m_addAll = addAll;
}

GitAddResult GitCommandAdd::execute()
{
    GitAddResult result;

    if (m_repositoryPath.trimmed().isEmpty()) {
        result.errorMessage = QStringLiteral("Repository path is empty.");
        return result;
    }

    if (!m_addAll && m_files.isEmpty()) {
        result.errorMessage = QStringLiteral("No files were specified for git add.");
        return result;
    }

    const GitProcessResult processResult = executeGitCommand(
        m_repositoryPath,
        buildArguments()
        );

    result.success = processResult.success;
    result.errorMessage = processResult.errorMessage;

    return result;
}

/***************************************************************************/
/*                                                                         */
/*                          Private Helpers                                */
/*                                                                         */
/***************************************************************************/

QStringList GitCommandAdd::buildArguments() const
{
    QStringList arguments;
    arguments << QStringLiteral("add");

    if (m_addAll) {
        arguments << QStringLiteral("--all");
        return arguments;
    }

    arguments << QStringLiteral("--");
    arguments << m_files;

    return arguments;
}
