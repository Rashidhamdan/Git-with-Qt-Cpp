#include "StatusWidget.h"

#include <QAbstractItemView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

/***************************************************************************/
/*                                                                         */
/*                            StatusWidget                                 */
/*                                                                         */
/***************************************************************************/

StatusWidget::StatusWidget(QWidget *parent)
    : QWidget(parent)
{
    initializeUi();
}

/***************************************************************************/
/*                                                                         */
/*                            ~StatusWidget                                */
/*                                                                         */
/***************************************************************************/

StatusWidget::~StatusWidget()
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void StatusWidget::setRepositoryName(const QString& repositoryName)
{
    if (m_repositoryLabel != nullptr) {
        if (repositoryName.trimmed().isEmpty()) {
            m_repositoryLabel->setText(tr("Repository: -"));
        } else {
            m_repositoryLabel->setText(tr("Repository: %1").arg(repositoryName));
        }
    }

    updateUiState();
}

void StatusWidget::setStagedFiles(const QStringList& files)
{
    if (m_stagedListWidget == nullptr) {
        return;
    }

    m_stagedListWidget->clear();
    m_stagedListWidget->addItems(files);

    updateGroupTitles();
    updateUiState();
}

void StatusWidget::setModifiedFiles(const QStringList& files)
{
    if (m_modifiedListWidget == nullptr) {
        return;
    }

    m_modifiedListWidget->clear();
    m_modifiedListWidget->addItems(files);

    updateGroupTitles();
    updateUiState();
}

void StatusWidget::setUntrackedFiles(const QStringList& files)
{
    if (m_untrackedListWidget == nullptr) {
        return;
    }

    m_untrackedListWidget->clear();
    m_untrackedListWidget->addItems(files);

    updateGroupTitles();
    updateUiState();
}

QStringList StatusWidget::selectedFiles() const
{
    QStringList files;

    auto appendSelected = [&files](const QListWidget* listWidget) {
        if (listWidget == nullptr) {
            return;
        }

        const QList<QListWidgetItem*> selectedItems = listWidget->selectedItems();
        for (QListWidgetItem* item : selectedItems) {
            if (item != nullptr) {
                const QString filePath = item->text().trimmed();
                if (!filePath.isEmpty() && !files.contains(filePath)) {
                    files.append(filePath);
                }
            }
        }
    };

    appendSelected(m_stagedListWidget);
    appendSelected(m_modifiedListWidget);
    appendSelected(m_untrackedListWidget);

    return files;
}

bool StatusWidget::hasStagedFiles() const
{
    return (m_stagedListWidget != nullptr && m_stagedListWidget->count() > 0);
}

bool StatusWidget::hasRepository() const
{
    if (m_repositoryLabel == nullptr) {
        return false;
    }

    return m_repositoryLabel->text() != tr("Repository: -");
}

void StatusWidget::clearStatus()
{
    if (m_stagedListWidget != nullptr) {
        m_stagedListWidget->clear();
    }

    if (m_modifiedListWidget != nullptr) {
        m_modifiedListWidget->clear();
    }

    if (m_untrackedListWidget != nullptr) {
        m_untrackedListWidget->clear();
    }

    updateGroupTitles();
    updateUiState();
}

void StatusWidget::refreshView()
{
    updateGroupTitles();
    updateUiState();
}

/***************************************************************************/
/*                                                                         */
/*                         UI Initialization                               */
/*                                                                         */
/***************************************************************************/

void StatusWidget::initializeUi()
{
    createWidgets();
    createLayouts();
    setupConnections();
    updateGroupTitles();
    updateUiState();
}

/***************************************************************************/
/*                                                                         */
/*                           Create Widgets                                */
/*                                                                         */
/***************************************************************************/

void StatusWidget::createWidgets()
{
    m_repositoryLabel = new QLabel(tr("Repository: -"), this);

    m_addSelectedButton = new QPushButton(tr("Add Selected"), this);
    m_commitButton = new QPushButton(tr("Commit"), this);
    m_pushButton = new QPushButton(tr("Push"), this);
    m_pullButton = new QPushButton(tr("Pull"), this);
    m_refreshButton = new QPushButton(tr("Refresh"), this);

    m_stagedGroupBox = new QGroupBox(tr("Staged Files"), this);
    m_modifiedGroupBox = new QGroupBox(tr("Modified Files"), this);
    m_untrackedGroupBox = new QGroupBox(tr("Untracked Files"), this);

    m_stagedListWidget = new QListWidget(this);
    m_modifiedListWidget = new QListWidget(this);
    m_untrackedListWidget = new QListWidget(this);

    m_stagedListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_modifiedListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_untrackedListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

/***************************************************************************/
/*                                                                         */
/*                           Create Layouts                                */
/*                                                                         */
/***************************************************************************/

void StatusWidget::createLayouts()
{
    auto* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(m_repositoryLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_addSelectedButton);
    headerLayout->addWidget(m_commitButton);
    headerLayout->addWidget(m_pushButton);
    headerLayout->addWidget(m_pullButton);
    headerLayout->addWidget(m_refreshButton);

    auto* stagedLayout = new QVBoxLayout();
    stagedLayout->addWidget(m_stagedListWidget);
    m_stagedGroupBox->setLayout(stagedLayout);

    auto* modifiedLayout = new QVBoxLayout();
    modifiedLayout->addWidget(m_modifiedListWidget);
    m_modifiedGroupBox->setLayout(modifiedLayout);

    auto* untrackedLayout = new QVBoxLayout();
    untrackedLayout->addWidget(m_untrackedListWidget);
    m_untrackedGroupBox->setLayout(untrackedLayout);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->addLayout(headerLayout);
    m_mainLayout->addWidget(m_stagedGroupBox);
    m_mainLayout->addWidget(m_modifiedGroupBox);
    m_mainLayout->addWidget(m_untrackedGroupBox);

    setLayout(m_mainLayout);
}

/***************************************************************************/
/*                                                                         */
/*                         Setup Connections                               */
/*                                                                         */
/***************************************************************************/

void StatusWidget::setupConnections()
{
    connect(m_refreshButton, &QPushButton::clicked, this, &StatusWidget::refreshRequested);

    connect(m_addSelectedButton, &QPushButton::clicked, this, [this]() {
        emit addSelectedRequested(selectedFiles());
    });

    connect(m_commitButton, &QPushButton::clicked, this, &StatusWidget::commitRequested);
    connect(m_pushButton, &QPushButton::clicked, this, &StatusWidget::pushRequested);
    connect(m_pullButton, &QPushButton::clicked, this, &StatusWidget::pullRequested);

    auto connectSelectionChanged = [this](QListWidget* listWidget) {
        if (listWidget == nullptr) {
            return;
        }

        connect(listWidget, &QListWidget::itemSelectionChanged, this, [this]() {
            updateUiState();
        });
    };

    connectSelectionChanged(m_stagedListWidget);
    connectSelectionChanged(m_modifiedListWidget);
    connectSelectionChanged(m_untrackedListWidget);
}

/***************************************************************************/
/*                                                                         */
/*                         Update Group Titles                             */
/*                                                                         */
/***************************************************************************/

void StatusWidget::updateGroupTitles()
{
    if (m_stagedGroupBox != nullptr && m_stagedListWidget != nullptr) {
        m_stagedGroupBox->setTitle(tr("Staged Files (%1)").arg(m_stagedListWidget->count()));
    }

    if (m_modifiedGroupBox != nullptr && m_modifiedListWidget != nullptr) {
        m_modifiedGroupBox->setTitle(tr("Modified Files (%1)").arg(m_modifiedListWidget->count()));
    }

    if (m_untrackedGroupBox != nullptr && m_untrackedListWidget != nullptr) {
        m_untrackedGroupBox->setTitle(tr("Untracked Files (%1)").arg(m_untrackedListWidget->count()));
    }
}

/***************************************************************************/
/*                                                                         */
/*                           Update UI State                               */
/*                                                                         */
/***************************************************************************/

void StatusWidget::updateUiState()
{
    const bool repositoryAvailable = hasRepository();
    const bool filesSelected = !selectedFiles().isEmpty();
    const bool stagedFilesAvailable = hasStagedFiles();

    if (m_addSelectedButton != nullptr) {
        m_addSelectedButton->setEnabled(repositoryAvailable && filesSelected);
    }

    if (m_commitButton != nullptr) {
        m_commitButton->setEnabled(repositoryAvailable && stagedFilesAvailable);
    }

    if (m_pushButton != nullptr) {
        m_pushButton->setEnabled(repositoryAvailable);
    }

    if (m_pullButton != nullptr) {
        m_pullButton->setEnabled(repositoryAvailable);
    }

    if (m_refreshButton != nullptr) {
        m_refreshButton->setEnabled(true);
    }
}
