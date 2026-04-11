#include "CommandConsoleWidget.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

/***************************************************************************/
/*                                                                         */
/*                       CommandConsoleWidget                              */
/*                                                                         */
/***************************************************************************/

CommandConsoleWidget::CommandConsoleWidget(QWidget *parent)
    : QWidget(parent)
{
    initializeUi();
}

CommandConsoleWidget::~CommandConsoleWidget()
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath;
    m_commandRouter.setRepositoryPath(repositoryPath);

    updateRepositoryInfo();
    updateUiState();
}

QString CommandConsoleWidget::repositoryPath() const
{
    return m_repositoryPath;
}

void CommandConsoleWidget::clearConsole()
{
    if (m_outputTextEdit != nullptr) {
        m_outputTextEdit->clear();
    }
}

void CommandConsoleWidget::appendMessage(const QString& message)
{
    if (m_outputTextEdit == nullptr) {
        return;
    }

    if (!message.isEmpty()) {
        m_outputTextEdit->appendPlainText(message);
    }
}

/***************************************************************************/
/*                                                                         */
/*                         UI Initialization                               */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::initializeUi()
{
    createWidgets();
    createLayouts();
    setupConnections();
    updateRepositoryInfo();
    updateUiState();

    appendSystemMessage(QStringLiteral("Hybrid CLI ready."));
    appendSystemMessage(QStringLiteral("Known commands use internal handlers. Unknown commands fall back to git passthrough."));
    appendSystemMessage(QStringLiteral("Type 'help' for supported commands or 'clear' to clear the console."));
}

/***************************************************************************/
/*                                                                         */
/*                           Create Widgets                                */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::createWidgets()
{
    m_repositoryLabel = new QLabel(tr("Repository: -"), this);
    m_repositoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_hintLabel = new QLabel(
        tr("Hybrid mode: implemented commands use the application logic, everything else falls back to git."),
        this
        );
    m_hintLabel->setWordWrap(true);

    m_outputTextEdit = new QPlainTextEdit(this);
    m_outputTextEdit->setReadOnly(true);

    QFont monoFont(QStringLiteral("Monospace"));
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setFixedPitch(true);
    monoFont.setPointSize(10);
    m_outputTextEdit->setFont(monoFont);

    m_inputLineEdit = new QLineEdit(this);
    m_inputLineEdit->setPlaceholderText(
        tr("Enter command, e.g. status | add --all | commit -m \"message\" | git stash")
        );

    m_executeButton = new QPushButton(tr("Execute"), this);
    m_helpButton = new QPushButton(tr("Help"), this);
    m_clearButton = new QPushButton(tr("Clear"), this);
}

/***************************************************************************/
/*                                                                         */
/*                           Create Layouts                                */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::createLayouts()
{
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);
    buttonLayout->addWidget(m_inputLineEdit, 1);
    buttonLayout->addWidget(m_executeButton);
    buttonLayout->addWidget(m_helpButton);
    buttonLayout->addWidget(m_clearButton);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);
    m_mainLayout->addWidget(m_repositoryLabel);
    m_mainLayout->addWidget(m_hintLabel);
    m_mainLayout->addWidget(m_outputTextEdit, 1);
    m_mainLayout->addLayout(buttonLayout);

    setLayout(m_mainLayout);
}

/***************************************************************************/
/*                                                                         */
/*                         Setup Connections                               */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::setupConnections()
{
    connect(m_executeButton, &QPushButton::clicked, this, [this]() {
        if (m_inputLineEdit == nullptr) {
            return;
        }

        executeCommandText(m_inputLineEdit->text());
    });

    connect(m_inputLineEdit, &QLineEdit::returnPressed, this, [this]() {
        if (m_inputLineEdit == nullptr) {
            return;
        }

        executeCommandText(m_inputLineEdit->text());
    });

    connect(m_inputLineEdit, &QLineEdit::textChanged, this, [this]() {
        updateUiState();
    });

    connect(m_helpButton, &QPushButton::clicked, this, [this]() {
        executeCommandText(QStringLiteral("help"));
    });

    connect(m_clearButton, &QPushButton::clicked, this, [this]() {
        clearConsole();
        appendSystemMessage(QStringLiteral("Console cleared."));
    });
}

/***************************************************************************/
/*                                                                         */
/*                         Update Repository Info                          */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::updateRepositoryInfo()
{
    if (m_repositoryLabel == nullptr) {
        return;
    }

    if (m_repositoryPath.trimmed().isEmpty()) {
        m_repositoryLabel->setText(tr("Repository: -"));
        m_repositoryLabel->setToolTip(QString());
    } else {
        m_repositoryLabel->setText(tr("Repository: %1").arg(m_repositoryPath));
        m_repositoryLabel->setToolTip(m_repositoryPath);
    }
}

/***************************************************************************/
/*                                                                         */
/*                           Update UI State                               */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::updateUiState()
{
    const bool hasInput =
        (m_inputLineEdit != nullptr && !m_inputLineEdit->text().trimmed().isEmpty());

    if (m_executeButton != nullptr) {
        m_executeButton->setEnabled(hasInput);
    }

    if (m_helpButton != nullptr) {
        m_helpButton->setEnabled(true);
    }

    if (m_clearButton != nullptr) {
        m_clearButton->setEnabled(true);
    }
}

/***************************************************************************/
/*                                                                         */
/*                           Command Handling                              */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::executeCommandText(const QString& commandText)
{
    const QString trimmedCommand = commandText.trimmed();
    if (trimmedCommand.isEmpty()) {
        appendSystemMessage(QStringLiteral("No command entered."));
        return;
    }

    appendCommandLine(trimmedCommand);

    if (trimmedCommand.compare(QStringLiteral("clear"), Qt::CaseInsensitive) == 0) {
        clearConsole();
        appendSystemMessage(QStringLiteral("Console cleared."));
        if (m_inputLineEdit != nullptr) {
            m_inputLineEdit->clear();
        }
        updateUiState();
        return;
    }

    const CommandParseResult parseResult = m_commandParser.parse(trimmedCommand);

    if (!parseResult.success) {
        appendErrorMessage(parseResult.errorMessage.isEmpty()
                           ? QStringLiteral("The command could not be parsed.")
                           : parseResult.errorMessage,
                           false);
        if (m_inputLineEdit != nullptr) {
            m_inputLineEdit->clear();
        }
        updateUiState();
        return;
    }

    const CommandRouteResult routeResult = m_commandRouter.route(parseResult);

    if (!routeResult.success) {
        appendErrorMessage(routeResult.errorMessage.isEmpty()
                           ? QStringLiteral("The command failed.")
                           : routeResult.errorMessage,
                           routeResult.usedPassthrough);
    } else {
        appendSuccessMessage(routeResult.outputMessage.isEmpty()
                             ? QStringLiteral("Command executed successfully.")
                             : routeResult.outputMessage,
                             routeResult.usedPassthrough);

        if (routeResult.repositoryStateChanged) {
            emit repositoryStateChanged();
        }
    }

    if (m_inputLineEdit != nullptr) {
        m_inputLineEdit->clear();
    }

    updateUiState();
}

/***************************************************************************/
/*                                                                         */
/*                             Output Helpers                              */
/*                                                                         */
/***************************************************************************/

void CommandConsoleWidget::appendCommandLine(const QString& commandText)
{
    appendMessage(QStringLiteral("> %1").arg(commandText));
}

void CommandConsoleWidget::appendSystemMessage(const QString& message)
{
    appendMessage(QStringLiteral("[INFO] %1").arg(message));
}

void CommandConsoleWidget::appendSuccessMessage(const QString& message, bool usedPassthrough)
{
    const QString source = usedPassthrough
                               ? QStringLiteral("git passthrough")
                               : QStringLiteral("internal");

    appendMessage(QStringLiteral("[OK] (%1)").arg(source));

    if (!message.trimmed().isEmpty()) {
        appendMessage(message);
    }

    appendMessage(QString());
}

void CommandConsoleWidget::appendErrorMessage(const QString& message, bool usedPassthrough)
{
    const QString source = usedPassthrough
                               ? QStringLiteral("git passthrough")
                               : QStringLiteral("internal");

    appendMessage(QStringLiteral("[ERROR] (%1)").arg(source));

    if (!message.trimmed().isEmpty()) {
        appendMessage(message);
    }

    appendMessage(QString());
}
