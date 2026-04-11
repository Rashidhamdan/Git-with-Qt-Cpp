#ifndef COMMANDCONSOLEWIDGET_H
#define COMMANDCONSOLEWIDGET_H

#include <QWidget>
#include <QString>

#include "CommandParser.h"
#include "CommandRouter.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QPlainTextEdit;
class QVBoxLayout;

/***************************************************************************/
/*                                                                         */
/*                       CommandConsoleWidget                              */
/*                                                                         */
/***************************************************************************/

class CommandConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommandConsoleWidget(QWidget *parent = nullptr);
    ~CommandConsoleWidget();

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    void clearConsole();
    void appendMessage(const QString& message);

signals:
    void repositoryStateChanged();

private:
    QString m_repositoryPath;

    QLabel* m_repositoryLabel = nullptr;
    QLabel* m_hintLabel = nullptr;

    QPlainTextEdit* m_outputTextEdit = nullptr;
    QLineEdit* m_inputLineEdit = nullptr;

    QPushButton* m_executeButton = nullptr;
    QPushButton* m_helpButton = nullptr;
    QPushButton* m_clearButton = nullptr;

    QVBoxLayout* m_mainLayout = nullptr;

    CommandParser m_commandParser;
    CommandRouter m_commandRouter;

private:
    void initializeUi();
    void createWidgets();
    void createLayouts();
    void setupConnections();
    void updateRepositoryInfo();
    void updateUiState();

    void executeCommandText(const QString& commandText);
    void appendCommandLine(const QString& commandText);
    void appendSystemMessage(const QString& message);
    void appendSuccessMessage(const QString& message, bool usedPassthrough);
    void appendErrorMessage(const QString& message, bool usedPassthrough);
};

#endif // COMMANDCONSOLEWIDGET_H
