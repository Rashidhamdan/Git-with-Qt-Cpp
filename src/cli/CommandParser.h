#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QString>
#include <QStringList>

/***************************************************************************/
/*                                                                         */
/*                       CommandParseResult                                */
/*                                                                         */
/***************************************************************************/

struct CommandParseResult
{
    bool success = false;
    QString originalInput;
    QString commandName;
    QStringList arguments;
    QString errorMessage;
    bool explicitGitPassthrough = false;
};

/***************************************************************************/
/*                                                                         */
/*                         CommandParser                                   */
/*                                                                         */
/***************************************************************************/

class CommandParser
{
public:
    CommandParser();

    CommandParseResult parse(const QString& input) const;

private:
    QStringList tokenize(const QString& input, QString* errorMessage = nullptr) const;
};

#endif // COMMANDPARSER_H
