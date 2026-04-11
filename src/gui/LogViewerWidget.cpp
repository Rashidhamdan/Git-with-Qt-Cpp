#include "LogViewerWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>

/***************************************************************************/
/*                                                                         */
/*                          LogViewerWidget                                */
/*                                                                         */
/***************************************************************************/

LogViewerWidget::LogViewerWidget(QWidget *parent)
    : QWidget(parent)
{
    initializeUi();
}

/***************************************************************************/
/*                                                                         */
/*                         ~LogViewerWidget                                */
/*                                                                         */
/***************************************************************************/

LogViewerWidget::~LogViewerWidget()
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void LogViewerWidget::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath;
    updateHeaderInfo();
}

QString LogViewerWidget::repositoryPath() const
{
    return m_repositoryPath;
}

void LogViewerWidget::setLogEntries(const QList<GitLogEntry>& entries)
{
    m_logEntries = entries;
    updateCommitList();
    refreshView();
}

QList<GitLogEntry> LogViewerWidget::logEntries() const
{
    return m_logEntries;
}

void LogViewerWidget::clearViewer()
{
    m_logEntries.clear();

    if (m_commitListWidget != nullptr) {
        m_commitListWidget->clear();
    }

    if (m_commitDetailsTextEdit != nullptr) {
        m_commitDetailsTextEdit->clear();
        m_commitDetailsTextEdit->setHtml(
            "<div style='color: #8b949e; font-style: italic; padding: 8px;'>"
            + tr("No commit selected.").toHtmlEscaped() +
            "</div>"
            );
    }
}

void LogViewerWidget::refreshView()
{
    updateHeaderInfo();

    if (m_commitListWidget == nullptr || m_commitDetailsTextEdit == nullptr) {
        return;
    }

    if (m_logEntries.isEmpty()) {
        showPlaceholderMessage(tr("No commits available."));
        return;
    }

    if (m_commitListWidget->currentRow() < 0 && m_commitListWidget->count() > 0) {
        m_commitListWidget->setCurrentRow(0);
    } else {
        showCommitDetails(m_commitListWidget->currentRow());
    }
}

void LogViewerWidget::showPlaceholderMessage(const QString& message)
{
    if (m_commitDetailsTextEdit == nullptr) {
        return;
    }

    const QString placeholderMessage =
        message.trimmed().isEmpty()
            ? tr("No commits available.")
            : message;

    m_commitDetailsTextEdit->setHtml(
        "<div style='color: #8b949e; font-style: italic; padding: 8px;'>"
        + placeholderMessage.toHtmlEscaped() +
        "</div>"
        );
}

/***************************************************************************/
/*                                                                         */
/*                        UI Initialization                                */
/*                                                                         */
/***************************************************************************/

void LogViewerWidget::initializeUi()
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

void LogViewerWidget::createWidgets()
{
    m_repositoryLabel = new QLabel(tr("Repository: -"), this);
    m_repositoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_refreshButton = new QPushButton(tr("Refresh"), this);

    m_commitListWidget = new QListWidget(this);
    m_commitListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    m_commitDetailsTextEdit = new QTextEdit(this);
    m_commitDetailsTextEdit->setReadOnly(true);
    m_commitDetailsTextEdit->setAcceptRichText(true);

    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->addWidget(m_commitListWidget);
    m_mainSplitter->addWidget(m_commitDetailsTextEdit);
    m_mainSplitter->setStretchFactor(0, 2);
    m_mainSplitter->setStretchFactor(1, 3);
    m_mainSplitter->setSizes({350, 550});
}

/***************************************************************************/
/*                                                                         */
/*                         Create Layouts                                  */
/*                                                                         */
/***************************************************************************/

void LogViewerWidget::createLayouts()
{
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);
    headerLayout->addWidget(m_repositoryLabel, 1);
    headerLayout->addWidget(m_refreshButton, 0);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);
    m_mainLayout->addLayout(headerLayout);
    m_mainLayout->addWidget(m_mainSplitter, 1);

    setLayout(m_mainLayout);
}

/***************************************************************************/
/*                                                                         */
/*                         Setup Connections                               */
/*                                                                         */
/***************************************************************************/

void LogViewerWidget::setupConnections()
{
    connect(m_refreshButton, &QPushButton::clicked, this, &LogViewerWidget::refreshRequested);

    connect(m_commitListWidget, &QListWidget::currentRowChanged,
            this, &LogViewerWidget::showCommitDetails);
}

/***************************************************************************/
/*                                                                         */
/*                        Update Header Info                               */
/*                                                                         */
/***************************************************************************/

void LogViewerWidget::updateHeaderInfo()
{
    if (m_repositoryLabel == nullptr) {
        return;
    }

    if (m_repositoryPath.isEmpty()) {
        m_repositoryLabel->setText(tr("Repository: -"));
        m_repositoryLabel->setToolTip(QString());
    } else {
        m_repositoryLabel->setText(tr("Repository: %1").arg(m_repositoryPath));
        m_repositoryLabel->setToolTip(m_repositoryPath);
    }
}

/***************************************************************************/
/*                                                                         */
/*                         Update Commit List                              */
/*                                                                         */
/***************************************************************************/

void LogViewerWidget::updateCommitList()
{
    if (m_commitListWidget == nullptr) {
        return;
    }

    m_commitListWidget->clear();

    for (const GitLogEntry& entry : std::as_const(m_logEntries)) {
        QString itemText = QString("%1  %2").arg(entry.shortHash, entry.subject);

        if (!entry.decorations.isEmpty()) {
            itemText += QString("  %1").arg(entry.decorations);
        }

        auto* item = new QListWidgetItem(itemText, m_commitListWidget);
        item->setToolTip(entry.fullHash);
    }
}

/***************************************************************************/
/*                                                                         */
/*                         Show Commit Details                             */
/*                                                                         */
/***************************************************************************/

void LogViewerWidget::showCommitDetails(int index)
{
    if (m_commitDetailsTextEdit == nullptr) {
        return;
    }

    if (index < 0 || index >= m_logEntries.size()) {
        showPlaceholderMessage(tr("No commit selected."));
        return;
    }

    const GitLogEntry& entry = m_logEntries.at(index);

    QString decorationsHtml;
    if (!entry.decorations.trimmed().isEmpty()) {
        QString decorations = entry.decorations.toHtmlEscaped();

        decorations.replace("HEAD", "<span style='color: #3fb950; font-weight: bold;'>HEAD</span>");
        decorations.replace("origin/", "<span style='color: #58a6ff;'>origin/</span>");
        decorations.replace("tag:", "<span style='color: #bc8cff; font-weight: bold;'>tag:</span>");

        decorationsHtml =
            "<div style='margin-top: 8px; color: #8b949e;'>"
            "<span style='font-weight: bold;'>Refs:</span> "
            + decorations +
            "</div>";
    }

    const QString html =
        "<div style='"
        "font-family: Segoe UI, Arial, sans-serif;"
        "color: #c9d1d9;"
        "padding: 8px;"
        "line-height: 1.5;"
        "'>"

        "<div style='margin-bottom: 12px;'>"
        "<span style='color: #8b949e; font-weight: bold;'>Commit:</span> "
        "<span style='color: #d29922; font-family: monospace; font-weight: bold;'>"
        + entry.fullHash.toHtmlEscaped() +
        "</span>"
        "</div>"

        "<div style='margin-bottom: 6px;'>"
        "<span style='color: #8b949e; font-weight: bold;'>Short Hash:</span> "
        "<span style='color: #d29922; font-family: monospace;'>"
        + entry.shortHash.toHtmlEscaped() +
        "</span>"
        "</div>"

        "<div style='margin-bottom: 6px;'>"
        "<span style='color: #8b949e; font-weight: bold;'>Author:</span> "
        "<span style='color: #c9d1d9;'>"
        + entry.authorName.toHtmlEscaped() +
        "</span>"
        "</div>"

        + (entry.authorEmail.isEmpty()
               ? QString()
               : "<div style='margin-bottom: 6px;'>"
                 "<span style='color: #8b949e; font-weight: bold;'>Email:</span> "
                 "<span style='color: #8b949e;'>"
                     + entry.authorEmail.toHtmlEscaped() +
                     "</span>"
                     "</div>")

        + (entry.authorDate.isEmpty()
               ? QString()
               : "<div style='margin-bottom: 6px;'>"
                 "<span style='color: #8b949e; font-weight: bold;'>Date:</span> "
                 "<span style='color: #8b949e;'>"
                     + entry.authorDate.toHtmlEscaped() +
                     "</span>"
                     "</div>")

        + decorationsHtml +

        "<hr style='border: none; border-top: 1px solid #30363d; margin: 14px 0;'>"

        "<div style='margin-bottom: 6px; color: #8b949e; font-weight: bold;'>Message:</div>"
        "<div style='"
        "color: #c9d1d9;"
        "background-color: #161b22;"
        "border: 1px solid #30363d;"
        "border-radius: 6px;"
        "padding: 10px;"
        "white-space: pre-wrap;"
        "'>"
        + entry.subject.toHtmlEscaped() +
        "</div>"

        "</div>";

    m_commitDetailsTextEdit->setHtml(html);

    emit commitSelected(entry.fullHash);
}
