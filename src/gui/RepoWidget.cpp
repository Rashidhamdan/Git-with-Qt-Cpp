#include "RepoWidget.h"

#include "CommandConsoleWidget.h"
#include "GitCommandAdd.h"
#include "GitCommandCommit.h"
#include "GitCommandDiff.h"
#include "GitCommandLog.h"
#include "GitCommandPull.h"
#include "GitCommandPush.h"
#include "GitCommandStatus.h"
#include "GitRepositoryInfo.h"

#include <QAbstractItemView>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSet>
#include <QSplitter>
#include <QTabWidget>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

/***************************************************************************/
/*                                                                         */
/*                           Local Helpers                                 */
/*                                                                         */
/***************************************************************************/

namespace
{
QString buildLogText(const QList<GitLogEntry>& entries)
{
    if (entries.isEmpty()) {
        return QStringLiteral("No commits available.");
    }

    QStringList lines;

    for (const GitLogEntry& entry : entries) {
        lines << QStringLiteral("commit %1").arg(entry.fullHash);

        if (!entry.decorations.trimmed().isEmpty()) {
            lines << QStringLiteral("refs   %1").arg(entry.decorations.trimmed());
        }

        lines << QStringLiteral("author %1 <%2>")
                     .arg(entry.authorName.trimmed(),
                          entry.authorEmail.trimmed());

        lines << QStringLiteral("date   %1").arg(entry.authorDate.trimmed());
        lines << QString();
        lines << QStringLiteral("    %1").arg(entry.subject.trimmed());
        lines << QString();
    }

    return lines.join(QChar('\n')).trimmed();
}
}

/***************************************************************************/
/*                                                                         */
/*                            RepoWidget                                   */
/*                                                                         */
/***************************************************************************/

RepoWidget::RepoWidget(const QString& repositoryPath, QWidget* parent)
    : QWidget(parent)
    , m_repositoryPath(repositoryPath)
{
    initializeUi();
    setRepositoryPath(repositoryPath);
}

RepoWidget::~RepoWidget()
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void RepoWidget::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath.trimmed();

    if (m_fileSystemModel != nullptr) {
        const QString rootPath = m_repositoryPath.isEmpty()
        ? QDir::homePath()
        : m_repositoryPath;

        const QModelIndex rootIndex = m_fileSystemModel->setRootPath(rootPath);

        if (m_repoTreeView != nullptr) {
            m_repoTreeView->setRootIndex(rootIndex);
        }
    }

    if (m_commandConsoleWidget != nullptr) {
        m_commandConsoleWidget->setRepositoryPath(m_repositoryPath);
    }

    refreshUi();
}

QString RepoWidget::repositoryPath() const
{
    return m_repositoryPath;
}

void RepoWidget::refreshUi()
{
    updateRepositoryLabel();
    refreshStatusView();
    refreshLogView();

    const QString diffPath = currentDiffCandidatePath();
    if (diffPath.trimmed().isEmpty()) {
        clearDiffView();
    } else {
        showDiffForPath(diffPath);
    }

    updateUiState();
}

bool RepoWidget::hasStagedChanges() const
{
    return !m_stagedFiles.isEmpty();
}

bool RepoWidget::stageSelectedFiles(QString* errorMessage)
{
    const QStringList selectedPaths = selectedRepositoryPaths();

    if (selectedPaths.isEmpty()) {
        setOptionalString(errorMessage, tr("No files or folders are selected."));
        return false;
    }

    GitCommandAdd gitCommandAdd(m_repositoryPath);
    gitCommandAdd.setAddAll(false);
    gitCommandAdd.setFiles(selectedPaths);

    const GitAddResult result = gitCommandAdd.execute();

    if (!result.success) {
        setOptionalString(
            errorMessage,
            result.errorMessage.isEmpty()
                ? tr("The selected files could not be staged.")
                : result.errorMessage
            );
        return false;
    }

    refreshUi();
    emit repositoryStateChanged();
    return true;
}

bool RepoWidget::stageAllChanges(QString* errorMessage)
{
    GitCommandAdd gitCommandAdd(m_repositoryPath);
    gitCommandAdd.setAddAll(true);

    const GitAddResult result = gitCommandAdd.execute();

    if (!result.success) {
        setOptionalString(
            errorMessage,
            result.errorMessage.isEmpty()
                ? tr("The changes could not be staged.")
                : result.errorMessage
            );
        return false;
    }

    refreshUi();
    emit repositoryStateChanged();
    return true;
}

bool RepoWidget::createCommit(const QString& message,
                              QString* errorMessage,
                              QString* commitHash)
{
    GitCommandCommit gitCommandCommit(m_repositoryPath);
    gitCommandCommit.setMessage(message);

    const GitCommitResult result = gitCommandCommit.execute();

    if (!result.success) {
        setOptionalString(
            errorMessage,
            result.errorMessage.isEmpty()
                ? tr("The commit could not be created.")
                : result.errorMessage
            );
        setOptionalString(commitHash, QString());
        return false;
    }

    setOptionalString(commitHash, result.commitHash);

    refreshUi();
    emit repositoryStateChanged();
    return true;
}

bool RepoWidget::pushChanges(QString* errorMessage,
                             QString* outputMessage)
{
    const QString upstream = GitRepositoryInfo::upstream(m_repositoryPath).trimmed();

    QString remoteName;
    QString branchName;

    if (!splitUpstreamReference(upstream, &remoteName, &branchName)) {
        setOptionalString(
            errorMessage,
            tr("No valid upstream is configured for the current branch.")
            );
        setOptionalString(outputMessage, QString());
        return false;
    }

    GitCommandPush gitCommandPush(m_repositoryPath);
    gitCommandPush.setRemote(remoteName);
    gitCommandPush.setBranch(branchName);

    const GitPushResult result = gitCommandPush.execute();

    if (!result.success) {
        setOptionalString(
            errorMessage,
            result.errorMessage.isEmpty()
                ? tr("The push operation failed.")
                : result.errorMessage
            );
        setOptionalString(outputMessage, QString());
        return false;
    }

    setOptionalString(outputMessage, result.outputMessage);

    refreshUi();
    emit repositoryStateChanged();
    return true;
}

bool RepoWidget::pullChanges(QString* errorMessage,
                             QString* outputMessage)
{
    const QString upstream = GitRepositoryInfo::upstream(m_repositoryPath).trimmed();

    QString remoteName;
    QString branchName;

    if (!splitUpstreamReference(upstream, &remoteName, &branchName)) {
        setOptionalString(
            errorMessage,
            tr("No valid upstream is configured for the current branch.")
            );
        setOptionalString(outputMessage, QString());
        return false;
    }

    GitCommandPull gitCommandPull(m_repositoryPath);
    gitCommandPull.setRemote(remoteName);
    gitCommandPull.setBranch(branchName);

    const GitPullResult result = gitCommandPull.execute();

    if (!result.success) {
        setOptionalString(
            errorMessage,
            result.errorMessage.isEmpty()
                ? tr("The pull operation failed.")
                : result.errorMessage
            );
        setOptionalString(outputMessage, QString());
        return false;
    }

    setOptionalString(outputMessage, result.outputMessage);

    refreshUi();
    emit repositoryStateChanged();
    return true;
}

/***************************************************************************/
/*                                                                         */
/*                         UI Initialization                               */
/*                                                                         */
/***************************************************************************/

void RepoWidget::initializeUi()
{
    createWidgets();
    createLayouts();
    setupConnections();
    updateRepositoryLabel();
    clearDiffView();
    updateUiState();
}

void RepoWidget::createWidgets()
{
    m_repositoryLabel = new QLabel(this);
    m_repositoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_refreshButton = new QPushButton(tr("Refresh"), this);
    m_addSelectedButton = new QPushButton(tr("Add Selected"), this);
    m_commitButton = new QPushButton(tr("Commit"), this);
    m_pushButton = new QPushButton(tr("Push"), this);
    m_pullButton = new QPushButton(tr("Pull"), this);
    m_refreshStatusButton = new QPushButton(tr("Refresh"), this);

    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    m_fileSystemModel = new QFileSystemModel(this);
    m_fileSystemModel->setReadOnly(true);
    m_fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    m_repoTreeView = new QTreeView(this);
    m_repoTreeView->setModel(m_fileSystemModel);
    m_repoTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_repoTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_repoTreeView->setSortingEnabled(true);
    m_repoTreeView->sortByColumn(0, Qt::AscendingOrder);
    m_repoTreeView->header()->setStretchLastSection(false);
    m_repoTreeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_repoTreeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_repoTreeView->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_repoTreeView->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    m_tabWidget = new QTabWidget(this);

    m_statusPage = new QWidget(this);
    m_logPage = new QWidget(this);
    m_diffPage = new QWidget(this);

    m_stagedListWidget = new QListWidget(this);
    m_modifiedListWidget = new QListWidget(this);
    m_untrackedListWidget = new QListWidget(this);

    m_stagedListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_modifiedListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_untrackedListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_logTextEdit = new QPlainTextEdit(this);
    m_logTextEdit->setReadOnly(true);

    m_diffTextEdit = new QPlainTextEdit(this);
    m_diffTextEdit->setReadOnly(true);

    m_commandConsoleWidget = new CommandConsoleWidget(this);

    m_tabWidget->addTab(m_statusPage, tr("Status"));
    m_tabWidget->addTab(m_logPage, tr("Log"));
    m_tabWidget->addTab(m_diffPage, tr("Diff"));
    m_tabWidget->addTab(m_commandConsoleWidget, tr("Console"));
}

void RepoWidget::createLayouts()
{
    auto* leftLayout = new QVBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(8);
    leftLayout->addWidget(m_repositoryLabel);

    auto* leftButtonLayout = new QHBoxLayout();
    leftButtonLayout->setContentsMargins(0, 0, 0, 0);
    leftButtonLayout->setSpacing(8);
    leftButtonLayout->addStretch();
    leftButtonLayout->addWidget(m_refreshButton);

    leftLayout->addLayout(leftButtonLayout);
    leftLayout->addWidget(m_repoTreeView, 1);

    auto* leftContainer = new QWidget(this);
    leftContainer->setLayout(leftLayout);

    auto* statusTopLayout = new QHBoxLayout();
    statusTopLayout->setContentsMargins(0, 0, 0, 0);
    statusTopLayout->setSpacing(8);
    statusTopLayout->addStretch();
    statusTopLayout->addWidget(m_addSelectedButton);
    statusTopLayout->addWidget(m_commitButton);
    statusTopLayout->addWidget(m_pushButton);
    statusTopLayout->addWidget(m_pullButton);
    statusTopLayout->addWidget(m_refreshStatusButton);

    auto* stagedGroup = new QGroupBox(tr("Staged Files"), m_statusPage);
    auto* modifiedGroup = new QGroupBox(tr("Modified Files"), m_statusPage);
    auto* untrackedGroup = new QGroupBox(tr("Untracked Files"), m_statusPage);

    auto* stagedLayout = new QVBoxLayout(stagedGroup);
    stagedLayout->setContentsMargins(8, 8, 8, 8);
    stagedLayout->addWidget(m_stagedListWidget);

    auto* modifiedLayout = new QVBoxLayout(modifiedGroup);
    modifiedLayout->setContentsMargins(8, 8, 8, 8);
    modifiedLayout->addWidget(m_modifiedListWidget);

    auto* untrackedLayout = new QVBoxLayout(untrackedGroup);
    untrackedLayout->setContentsMargins(8, 8, 8, 8);
    untrackedLayout->addWidget(m_untrackedListWidget);

    auto* statusLayout = new QVBoxLayout(m_statusPage);
    statusLayout->setContentsMargins(8, 8, 8, 8);
    statusLayout->setSpacing(10);
    statusLayout->addLayout(statusTopLayout);
    statusLayout->addWidget(stagedGroup, 1);
    statusLayout->addWidget(modifiedGroup, 1);
    statusLayout->addWidget(untrackedGroup, 1);

    auto* logLayout = new QVBoxLayout(m_logPage);
    logLayout->setContentsMargins(8, 8, 8, 8);
    logLayout->addWidget(m_logTextEdit);

    auto* diffLayout = new QVBoxLayout(m_diffPage);
    diffLayout->setContentsMargins(8, 8, 8, 8);
    diffLayout->addWidget(m_diffTextEdit);

    m_mainSplitter->addWidget(leftContainer);
    m_mainSplitter->addWidget(m_tabWidget);
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);
    mainLayout->addWidget(m_mainSplitter);

    setLayout(mainLayout);
}

void RepoWidget::setupConnections()
{
    connect(m_refreshButton, &QPushButton::clicked,
            this, &RepoWidget::refreshUi);

    connect(m_refreshStatusButton, &QPushButton::clicked,
            this, &RepoWidget::refreshUi);

    connect(m_addSelectedButton, &QPushButton::clicked, this, [this]() {
        QString errorMessage;

        if (!stageSelectedFiles(&errorMessage)) {
            QMessageBox::critical(
                this,
                tr("Git Add Failed"),
                errorMessage.isEmpty()
                    ? tr("The selected files could not be staged.")
                    : errorMessage
                );
        }
    });

    connect(m_commitButton, &QPushButton::clicked,
            this, &RepoWidget::commitRequestedFromUi);

    connect(m_pushButton, &QPushButton::clicked,
            this, &RepoWidget::pushRequestedFromUi);

    connect(m_pullButton, &QPushButton::clicked,
            this, &RepoWidget::pullRequestedFromUi);

    if (m_repoTreeView != nullptr && m_repoTreeView->selectionModel() != nullptr) {
        connect(m_repoTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, [this]() {
                    updateUiState();

                    const QString diffPath = currentDiffCandidatePath();
                    if (diffPath.trimmed().isEmpty()) {
                        clearDiffView();
                    } else {
                        showDiffForPath(diffPath);
                    }
                });
    }

    auto connectListWidget = [this](QListWidget* listWidget) {
        connect(listWidget, &QListWidget::itemSelectionChanged, this, [this]() {
            updateUiState();

            const QString diffPath = currentDiffCandidatePath();
            if (diffPath.trimmed().isEmpty()) {
                clearDiffView();
            } else {
                showDiffForPath(diffPath);
            }
        });
    };

    connectListWidget(m_stagedListWidget);
    connectListWidget(m_modifiedListWidget);
    connectListWidget(m_untrackedListWidget);

    if (m_commandConsoleWidget != nullptr) {
        connect(m_commandConsoleWidget, &CommandConsoleWidget::repositoryStateChanged,
                this, [this]() {
                    refreshUi();
                    emit repositoryStateChanged();
                });
    }
}

/***************************************************************************/
/*                                                                         */
/*                           UI Updates                                    */
/*                                                                         */
/***************************************************************************/

void RepoWidget::updateRepositoryLabel()
{
    if (m_repositoryLabel == nullptr) {
        return;
    }

    if (m_repositoryPath.trimmed().isEmpty()) {
        m_repositoryLabel->setText(tr("Repository: -"));
        m_repositoryLabel->setToolTip(QString());
        return;
    }

    m_repositoryLabel->setText(
        tr("Repository: %1").arg(QFileInfo(m_repositoryPath).fileName())
        );
    m_repositoryLabel->setToolTip(m_repositoryPath);
}

void RepoWidget::updateUiState()
{
    const bool hasRepository = !m_repositoryPath.trimmed().isEmpty();
    const bool hasSelection = !selectedRepositoryPaths().isEmpty();
    const bool upstreamAvailable = hasRepository && !GitRepositoryInfo::upstream(m_repositoryPath).trimmed().isEmpty();

    m_refreshButton->setEnabled(hasRepository);
    m_addSelectedButton->setEnabled(hasRepository && hasSelection);
    m_commitButton->setEnabled(hasRepository && hasStagedChanges());
    m_pushButton->setEnabled(upstreamAvailable);
    m_pullButton->setEnabled(upstreamAvailable);
    m_refreshStatusButton->setEnabled(hasRepository);
}

void RepoWidget::refreshStatusView()
{
    m_stagedFiles.clear();
    m_modifiedFiles.clear();
    m_untrackedFiles.clear();

    if (m_stagedListWidget != nullptr) {
        m_stagedListWidget->clear();
    }

    if (m_modifiedListWidget != nullptr) {
        m_modifiedListWidget->clear();
    }

    if (m_untrackedListWidget != nullptr) {
        m_untrackedListWidget->clear();
    }

    if (m_repositoryPath.trimmed().isEmpty()) {
        return;
    }

    GitCommandStatus gitCommandStatus(m_repositoryPath);
    const GitStatusResult result = gitCommandStatus.execute();

    if (!result.success) {
        return;
    }

    m_stagedFiles = result.stagedFiles;
    m_modifiedFiles = result.modifiedFiles;
    m_untrackedFiles = result.untrackedFiles;

    for (const QString& path : m_stagedFiles) {
        m_stagedListWidget->addItem(path);
    }

    for (const QString& path : m_modifiedFiles) {
        m_modifiedListWidget->addItem(path);
    }

    for (const QString& path : m_untrackedFiles) {
        m_untrackedListWidget->addItem(path);
    }
}

void RepoWidget::refreshLogView()
{
    if (m_logTextEdit == nullptr) {
        return;
    }

    m_logTextEdit->clear();

    if (m_repositoryPath.trimmed().isEmpty()) {
        return;
    }

    GitCommandLog gitCommandLog(m_repositoryPath);
    const GitLogResult result = gitCommandLog.execute();

    if (!result.success) {
        m_logTextEdit->setPlainText(
            result.errorMessage.isEmpty()
                ? tr("The commit history could not be loaded.")
                : result.errorMessage
            );
        return;
    }

    m_logTextEdit->setPlainText(buildLogText(result.entries));
}

void RepoWidget::clearDiffView()
{
    if (m_diffTextEdit == nullptr) {
        return;
    }

    m_diffTextEdit->setPlainText(
        tr("Select a file from the repository tree or the status lists to view its diff.")
        );
}

void RepoWidget::showDiffForPath(const QString& relativePath)
{
    if (m_diffTextEdit == nullptr) {
        return;
    }

    const QString normalizedPath = relativePath.trimmed();
    if (normalizedPath.isEmpty()) {
        clearDiffView();
        return;
    }

    GitCommandDiff gitCommandDiff(m_repositoryPath, normalizedPath);
    const GitDiffResult result = gitCommandDiff.execute();

    if (!result.success) {
        m_diffTextEdit->setPlainText(
            result.errorMessage.isEmpty()
                ? tr("The diff could not be loaded.")
                : result.errorMessage
            );
        return;
    }

    if (result.diffText.trimmed().isEmpty()) {
        m_diffTextEdit->setPlainText(
            tr("No diff output is available for \"%1\".").arg(normalizedPath)
            );
        return;
    }

    m_diffTextEdit->setPlainText(result.diffText);
}

/***************************************************************************/
/*                                                                         */
/*                            Helpers                                      */
/*                                                                         */
/***************************************************************************/

QStringList RepoWidget::selectedRepositoryPaths() const
{
    QSet<QString> uniquePaths;

    auto appendPath = [&uniquePaths](const QString& value) {
        const QString normalized = value.trimmed();
        if (!normalized.isEmpty()) {
            uniquePaths.insert(normalized);
        }
    };

    if (m_repoTreeView != nullptr && m_repoTreeView->selectionModel() != nullptr) {
        const QModelIndexList selectedRows = m_repoTreeView->selectionModel()->selectedRows(0);

        for (const QModelIndex& index : selectedRows) {
            const QString absolutePath = m_fileSystemModel->filePath(index);
            const QString relativePath = toRepositoryRelativePath(absolutePath);

            if (!relativePath.isEmpty() && relativePath != QStringLiteral(".")) {
                appendPath(relativePath);
            }
        }
    }

    if (m_stagedListWidget != nullptr) {
        const QList<QListWidgetItem*> items = m_stagedListWidget->selectedItems();
        for (QListWidgetItem* item : items) {
            if (item != nullptr) {
                appendPath(item->text());
            }
        }
    }

    if (m_modifiedListWidget != nullptr) {
        const QList<QListWidgetItem*> items = m_modifiedListWidget->selectedItems();
        for (QListWidgetItem* item : items) {
            if (item != nullptr) {
                appendPath(item->text());
            }
        }
    }

    if (m_untrackedListWidget != nullptr) {
        const QList<QListWidgetItem*> items = m_untrackedListWidget->selectedItems();
        for (QListWidgetItem* item : items) {
            if (item != nullptr) {
                appendPath(item->text());
            }
        }
    }

    QStringList result = uniquePaths.values();
    result.sort();
    return result;
}

QString RepoWidget::currentDiffCandidatePath() const
{
    auto extractCurrentItemPath = [](const QListWidget* listWidget) -> QString {
        if (listWidget == nullptr || listWidget->currentItem() == nullptr) {
            return QString();
        }

        return listWidget->currentItem()->text().trimmed();
    };

    const QString stagedPath = extractCurrentItemPath(m_stagedListWidget);
    if (!stagedPath.isEmpty()) {
        return stagedPath;
    }

    const QString modifiedPath = extractCurrentItemPath(m_modifiedListWidget);
    if (!modifiedPath.isEmpty()) {
        return modifiedPath;
    }

    const QString untrackedPath = extractCurrentItemPath(m_untrackedListWidget);
    if (!untrackedPath.isEmpty()) {
        return untrackedPath;
    }

    if (m_repoTreeView != nullptr && m_repoTreeView->selectionModel() != nullptr) {
        const QModelIndex currentIndex = m_repoTreeView->selectionModel()->currentIndex();
        if (currentIndex.isValid()) {
            const QString absolutePath = m_fileSystemModel->filePath(currentIndex);
            const QFileInfo fileInfo(absolutePath);

            if (fileInfo.exists() && fileInfo.isFile()) {
                return toRepositoryRelativePath(absolutePath);
            }
        }
    }

    return QString();
}

QString RepoWidget::toRepositoryRelativePath(const QString& absolutePath) const
{
    if (m_repositoryPath.trimmed().isEmpty() || absolutePath.trimmed().isEmpty()) {
        return QString();
    }

    const QDir repositoryDir(m_repositoryPath);
    QString relativePath = repositoryDir.relativeFilePath(absolutePath).trimmed();

    if (relativePath.isEmpty()) {
        return QString();
    }

    relativePath = QDir::cleanPath(relativePath);
    relativePath.replace(QChar('\\'), QChar('/'));

    if (relativePath == QStringLiteral(".")) {
        return QString();
    }

    return relativePath;
}

bool RepoWidget::splitUpstreamReference(const QString& upstream,
                                        QString* remoteName,
                                        QString* branchName)
{
    if (remoteName != nullptr) {
        remoteName->clear();
    }

    if (branchName != nullptr) {
        branchName->clear();
    }

    const QString trimmedUpstream = upstream.trimmed();
    if (trimmedUpstream.isEmpty()) {
        return false;
    }

    const int separatorIndex = trimmedUpstream.indexOf('/');
    if (separatorIndex <= 0 || separatorIndex >= trimmedUpstream.length() - 1) {
        return false;
    }

    const QString remote = trimmedUpstream.left(separatorIndex).trimmed();
    const QString branch = trimmedUpstream.mid(separatorIndex + 1).trimmed();

    if (remote.isEmpty() || branch.isEmpty()) {
        return false;
    }

    if (remoteName != nullptr) {
        *remoteName = remote;
    }

    if (branchName != nullptr) {
        *branchName = branch;
    }

    return true;
}

void RepoWidget::setOptionalString(QString* target, const QString& value)
{
    if (target != nullptr) {
        *target = value;
    }
}
