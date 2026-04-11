#include "CommandParser.h"

/***************************************************************************/
/*                                                                         */
/*                         CommandParser                                   */
/*                                                                         */
/***************************************************************************/

CommandParser::CommandParser()
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

CommandParseResult CommandParser::parse(const QString& input) const
{
    CommandParseResult result;
    result.originalInput = input;

    const QString trimmedInput = input.trimmed();
    if (trimmedInput.isEmpty()) {
        result.errorMessage = QStringLiteral("No command entered.");
        return result;
    }

    QString errorMessage;
    const QStringList tokens = tokenize(trimmedInput, &errorMessage);

    if (!errorMessage.isEmpty()) {
        result.errorMessage = errorMessage;
        return result;
    }

    if (tokens.isEmpty()) {
        result.errorMessage = QStringLiteral("No command entered.");
        return result;
    }

    const QString firstToken = tokens.first().trimmed();
    if (firstToken.isEmpty()) {
        result.errorMessage = QStringLiteral("No command entered.");
        return result;
    }

    result.success = true;
    result.commandName = firstToken.toLower();
    result.arguments = tokens.mid(1);

    if (result.commandName == QStringLiteral("git")) {
        result.explicitGitPassthrough = true;
    }

    return result;
}

/***************************************************************************/
/*                                                                         */
/*                              Tokenizing                                 */
/*                                                                         */
/***************************************************************************/

QStringList CommandParser::tokenize(const QString& input, QString* errorMessage) const
{
    if (errorMessage != nullptr) {
        errorMessage->clear();
    }

    QStringList tokens;
    QString currentToken;

    bool inSingleQuotes = false;
    bool inDoubleQuotes = false;
    bool escaping = false;

    for (int i = 0; i < input.length(); ++i) {
        const QChar ch = input.at(i);

        if (escaping) {
            currentToken.append(ch);
            escaping = false;
            continue;
        }

        if (ch == QChar('\\')) {
            escaping = true;
            continue;
        }

        if (ch == QChar('\'') && !inDoubleQuotes) {
            inSingleQuotes = !inSingleQuotes;
            continue;
        }

        if (ch == QChar('"') && !inSingleQuotes) {
            inDoubleQuotes = !inDoubleQuotes;
            continue;
        }

        if (!inSingleQuotes && !inDoubleQuotes && ch.isSpace()) {
            if (!currentToken.isEmpty()) {
                tokens.append(currentToken);
                currentToken.clear();
            }
            continue;
        }

        currentToken.append(ch);
    }

    if (escaping) {
        currentToken.append(QChar('\\'));
    }

    if (inSingleQuotes || inDoubleQuotes) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Unclosed quote in command.");
        }
        return {};
    }

    if (!currentToken.isEmpty()) {
        tokens.append(currentToken);
    }

    return tokens;
}
