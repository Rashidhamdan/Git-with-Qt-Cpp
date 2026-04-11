#include "DiffViewerWidget.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

/***************************************************************************/
/*                                                                         */
/*                          DiffViewerWidget                               */
/*                                                                         */
/***************************************************************************/

DiffViewerWidget::DiffViewerWidget(QWidget *parent)
    : QWidget(parent)
{
    initializeUi();
}

/***************************************************************************/
/*                                                                         */
/*                         ~DiffViewerWidget                               */
/*                                                                         */
/***************************************************************************/

DiffViewerWidget::~DiffViewerWidget()
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void DiffViewerWidget::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath;
    updateHeaderInfo();
}

QString DiffViewerWidget::repositoryPath() const
{
    return m_repositoryPath;
}

void DiffViewerWidget::setFilePath(const QString& filePath)
{
    if (m_filePath == filePath) {
        return;
    }

    m_filePath = filePath;
    m_diffText.clear();
    updateHeaderInfo();

    emit filePathChanged(m_filePath);
}

QString DiffViewerWidget::filePath() const
{
    return m_filePath;
}

void DiffViewerWidget::setDiffText(const QString& diffText)
{
    m_diffText = diffText;

    if (m_diffTextEdit == nullptr) {
        return;
    }

    if (m_diffText.trimmed().isEmpty()) {
        if (m_filePath.isEmpty()) {
            showPlaceholderMessage(tr("Select a file to view its diff."));
        } else {
            showPlaceholderMessage(tr("No changes in this file."));
        }
        return;
    }

    QString html;
    html += "<pre style='font-family: monospace;white-space: pre;padding: 8px;line-height: 1.2;color: #c9d1d9;margin: 0;'>";

    const QStringList lines = m_diffText.split('\n');

    int oldLineNumber = 0;
    int newLineNumber = 0;

    const QRegularExpression hunkHeaderRegex(
        QStringLiteral("^@@ -(\\d+)(?:,\\d+)? \\+(\\d+)(?:,\\d+)? @@")
        );

    auto formatLineNumber = [](int number) -> QString {
        if (number <= 0) {
            return QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;");
        }
        return QString::number(number).rightJustified(4, ' ');
    };

    for (const QString& line : lines) {
        const QString escaped = line.toHtmlEscaped();

        QString oldNumberHtml = QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;");
        QString newNumberHtml = QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;");
        QString contentHtml;

        if (line.startsWith("@@")) {
            const QRegularExpressionMatch match = hunkHeaderRegex.match(line);

            if (match.hasMatch()) {
                oldLineNumber = match.captured(1).toInt();
                newLineNumber = match.captured(2).toInt();
            }

            contentHtml =
                "<span style='color: #58a6ff; font-weight: bold;'>"
                + escaped +
                "</span>";
        }
        else if (line.startsWith('+') && !line.startsWith("+++")) {
            newNumberHtml = formatLineNumber(newLineNumber++);
            contentHtml =
                "<span style='color: #3fb950; background-color: #0d2d1a;'>"
                + escaped +
                "</span>";
        }
        else if (line.startsWith('-') && !line.startsWith("---")) {
            oldNumberHtml = formatLineNumber(oldLineNumber++);
            contentHtml =
                "<span style='color: #f85149; background-color: #2d0d0d;'>"
                + escaped +
                "</span>";
        }
        else if (line.startsWith("diff ")
                 || line.startsWith("index ")
                 || line.startsWith("--- ")
                 || line.startsWith("+++ ")) {
            contentHtml =
                "<span style='color: #8b949e;'>"
                + escaped +
                "</span>";
        }
        else {
            if (!line.isEmpty()) {
                if (oldLineNumber > 0) {
                    oldNumberHtml = formatLineNumber(oldLineNumber++);
                }
                if (newLineNumber > 0) {
                    newNumberHtml = formatLineNumber(newLineNumber++);
                }
            }

            contentHtml = escaped;
        }

        html += "<span style='color: #6e7681; display: inline-block; width: 90px; user-select: none;'>"
                + oldNumberHtml + " " + newNumberHtml +
                "</span>"
                + contentHtml
                + "<br>";
    }

    html += "</pre>";

    m_diffTextEdit->setHtml(html);
}

QString DiffViewerWidget::diffText() const
{
    return m_diffText;
}

void DiffViewerWidget::clearViewer()
{
    m_diffText.clear();
    m_filePath.clear();

    updateHeaderInfo();

    if (m_diffTextEdit != nullptr) {
        m_diffTextEdit->clear();
        m_diffTextEdit->setPlainText(tr("No file selected."));
    }
}

void DiffViewerWidget::showPlaceholderMessage(const QString& message)
{
    if (m_diffTextEdit == nullptr) {
        return;
    }

    const QString placeholderMessage =
        message.trimmed().isEmpty()
            ? tr("Select a file to view its diff.")
            : message;

    m_diffTextEdit->setHtml(
        "<div style='color: gray; font-style: italic; padding: 10px;'>"
        + placeholderMessage.toHtmlEscaped() +
        "</div>"
        );
}

void DiffViewerWidget::refreshView()
{
    updateHeaderInfo();

    if (m_diffTextEdit == nullptr) {
        return;
    }

    if (m_diffText.trimmed().isEmpty()) {
        if (!m_filePath.isEmpty()) {
            showPlaceholderMessage(tr("No changes in this file."));
        } else {
            showPlaceholderMessage(tr("Select a file to view its diff."));
        }
        return;
    }

    setDiffText(m_diffText);
}

/***************************************************************************/
/*                                                                         */
/*                        UI Initialization                                */
/*                                                                         */
/***************************************************************************/

void DiffViewerWidget::initializeUi()
{
    createWidgets();
    createLayouts();
    setupConnections();
    updateHeaderInfo();
    showPlaceholderMessage();
}

/***************************************************************************/
/*                                                                         */
/*                         Create Widgets                                  */
/*                                                                         */
/***************************************************************************/

void DiffViewerWidget::createWidgets()
{
    m_repositoryLabel = new QLabel(tr("Repository: -"), this);
    m_repositoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_filePathLabel = new QLabel(tr("File: -"), this);
    m_filePathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_refreshButton = new QPushButton(tr("Refresh"), this);

    m_diffTextEdit = new QTextEdit(this);
    m_diffTextEdit->setReadOnly(true);
    m_diffTextEdit->setLineWrapMode(QTextEdit::NoWrap);
    m_diffTextEdit->setAcceptRichText(true);

    QFont monoFont(QStringLiteral("Monospace"));
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setFixedPitch(true);
    monoFont.setPointSize(10);
    m_diffTextEdit->setFont(monoFont);
}

/***************************************************************************/
/*                                                                         */
/*                         Create Layouts                                  */
/*                                                                         */
/***************************************************************************/

void DiffViewerWidget::createLayouts()
{
    auto* topRowLayout = new QHBoxLayout();
    topRowLayout->setContentsMargins(0, 0, 0, 0);
    topRowLayout->setSpacing(8);
    topRowLayout->addWidget(m_repositoryLabel, 1);
    topRowLayout->addWidget(m_refreshButton, 0);

    auto* secondRowLayout = new QHBoxLayout();
    secondRowLayout->setContentsMargins(0, 0, 0, 0);
    secondRowLayout->setSpacing(8);
    secondRowLayout->addWidget(m_filePathLabel, 1);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);

    m_mainLayout->addLayout(topRowLayout);
    m_mainLayout->addLayout(secondRowLayout);
    m_mainLayout->addWidget(m_diffTextEdit, 1);

    setLayout(m_mainLayout);
}

/***************************************************************************/
/*                                                                         */
/*                         Setup Connections                               */
/*                                                                         */
/***************************************************************************/

void DiffViewerWidget::setupConnections()
{
    connect(m_refreshButton, &QPushButton::clicked, this, &DiffViewerWidget::refreshRequested);
}

/***************************************************************************/
/*                                                                         */
/*                        Update Header Info                               */
/*                                                                         */
/***************************************************************************/

void DiffViewerWidget::updateHeaderInfo()
{
    if (m_repositoryLabel != nullptr) {
        if (m_repositoryPath.isEmpty()) {
            m_repositoryLabel->setText(tr("Repository: -"));
            m_repositoryLabel->setToolTip(QString());
        } else {
            m_repositoryLabel->setText(tr("Repository: %1").arg(m_repositoryPath));
            m_repositoryLabel->setToolTip(m_repositoryPath);
        }
    }

    if (m_filePathLabel != nullptr) {
        if (m_filePath.isEmpty()) {
            m_filePathLabel->setText(tr("File: -"));
            m_filePathLabel->setToolTip(QString());
        } else {
            m_filePathLabel->setText(tr("File: %1").arg(m_filePath));
            m_filePathLabel->setToolTip(m_filePath);
        }
    }
}
