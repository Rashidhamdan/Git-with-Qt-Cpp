#include "BranchManagerDialog.h"

#include "GitRepositoryInfo.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

/***************************************************************************/
/*                                                                         */
/*                     BranchManagerDialog                                 */
/*                                                                         */
/***************************************************************************/

BranchManagerDialog::BranchManagerDialog(QWidget *parent)
    : QDialog(parent)
{
    initializeUi();
}

BranchManagerDialog::~BranchManagerDialog()
{
}

/***************************************************************************/
/*                                                                         */
/*                            Public API                                   */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::setRepositoryPath(const QString& repositoryPath)
{
    m_repositoryPath = repositoryPath.trimmed();

    if (m_commandConsoleWidget != nullptr) {
        m_commandConsoleWidget->setRepositoryPath(m_repositoryPath);
    }

    updateRepositoryInfo();
    refreshBranches();
}

QString BranchManagerDialog::repositoryPath() const
{
    return m_repositoryPath;
}

void BranchManagerDialog::refreshBranches()
{
    if (m_repositoryPath.isEmpty()) {
        if (m_localBranchesListWidget != nullptr) {
            m_localBranchesListWidget->clear();
        }

        if (m_remoteBranchesListWidget != nullptr) {
            m_remoteBranchesListWidget->clear();
        }

        updateRepositoryInfo();
        updateUiState();
        return;
    }

    GitCommandBranch gitCommandBranch(m_repositoryPath);
    const GitBranchResult result = gitCommandBranch.execute();

    if (!result.success) {
        if (m_localBranchesListWidget != nullptr) {
            m_localBranchesListWidget->clear();
        }

        if (m_remoteBranchesListWidget != nullptr) {
            m_remoteBranchesListWidget->clear();
        }

        QMessageBox::warning(
            this,
            tr("Branches Could Not Be Loaded"),
            result.errorMessage.isEmpty()
                ? tr("The branch list could not be loaded.")
                : result.errorMessage
            );

        updateRepositoryInfo();
        updateUiState();
        return;
    }

    loadBranchesIntoUi(result);
    selectCurrentLocalBranch();
    updateRepositoryInfo();
    updateUiState();
}

/***************************************************************************/
/*                                                                         */
/*                         UI Initialization                               */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::initializeUi()
{
    createWidgets();
    createLayouts();
    setupConnections();
    updateRepositoryInfo();
    updateUiState();

    setWindowTitle(tr("Branch Manager"));
    setModal(true);
    resize(900, 650);
}

/***************************************************************************/
/*                                                                         */
/*                           Create Widgets                                */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::createWidgets()
{
    m_repositoryLabel = new QLabel(tr("Repository: -"), this);
    m_repositoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_currentBranchLabel = new QLabel(tr("Current Branch: -"), this);
    m_currentBranchLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_tabWidget = new QTabWidget(this);

    m_branchPage = new QWidget(this);

    m_localBranchesListWidget = new QListWidget(this);
    m_remoteBranchesListWidget = new QListWidget(this);

    m_newBranchLineEdit = new QLineEdit(this);
    m_newBranchLineEdit->setPlaceholderText(tr("Enter new branch name"));

    m_refreshButton = new QPushButton(tr("Refresh"), this);
    m_checkoutLocalButton = new QPushButton(tr("Checkout Local"), this);
    m_checkoutRemoteButton = new QPushButton(tr("Checkout Remote Tracking"), this);
    m_createBranchButton = new QPushButton(tr("Create + Checkout"), this);
    m_closeButton = new QPushButton(tr("Close"), this);

    m_commandConsoleWidget = new CommandConsoleWidget(this);

    m_tabWidget->addTab(m_branchPage, tr("Branches"));
    m_tabWidget->addTab(m_commandConsoleWidget, tr("Console"));
}

/***************************************************************************/
/*                                                                         */
/*                           Create Layouts                                */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::createLayouts()
{
    auto* infoLayout = new QVBoxLayout();
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(6);
    infoLayout->addWidget(m_repositoryLabel);
    infoLayout->addWidget(m_currentBranchLabel);

    auto* listHeaderLayout = new QHBoxLayout();
    listHeaderLayout->setContentsMargins(0, 0, 0, 0);
    listHeaderLayout->setSpacing(12);

    auto* localLabel = new QLabel(tr("Local Branches"), this);
    auto* remoteLabel = new QLabel(tr("Remote Branches"), this);

    listHeaderLayout->addWidget(localLabel, 1);
    listHeaderLayout->addWidget(remoteLabel, 1);

    auto* listLayout = new QHBoxLayout();
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(12);
    listLayout->addWidget(m_localBranchesListWidget, 1);
    listLayout->addWidget(m_remoteBranchesListWidget, 1);

    auto* createLayout = new QHBoxLayout();
    createLayout->setContentsMargins(0, 0, 0, 0);
    createLayout->setSpacing(8);
    createLayout->addWidget(m_newBranchLineEdit, 1);
    createLayout->addWidget(m_createBranchButton);

    auto* actionLayout = new QHBoxLayout();
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(8);
    actionLayout->addWidget(m_refreshButton);
    actionLayout->addWidget(m_checkoutLocalButton);
    actionLayout->addWidget(m_checkoutRemoteButton);
    actionLayout->addStretch();
    actionLayout->addWidget(m_closeButton);

    auto* branchPageLayout = new QVBoxLayout(m_branchPage);
    branchPageLayout->setContentsMargins(8, 8, 8, 8);
    branchPageLayout->setSpacing(10);
    branchPageLayout->addLayout(infoLayout);
    branchPageLayout->addLayout(listHeaderLayout);
    branchPageLayout->addLayout(listLayout, 1);
    branchPageLayout->addLayout(createLayout);
    branchPageLayout->addLayout(actionLayout);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);
    m_mainLayout->addWidget(m_tabWidget);

    setLayout(m_mainLayout);
}

/***************************************************************************/
/*                                                                         */
/*                         Setup Connections                               */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::setupConnections()
{
    connect(m_refreshButton, &QPushButton::clicked,
            this, &BranchManagerDialog::refreshBranches);

    connect(m_closeButton, &QPushButton::clicked,
            this, &BranchManagerDialog::accept);

    connect(m_checkoutLocalButton, &QPushButton::clicked,
            this, &BranchManagerDialog::checkoutSelectedLocalBranch);

    connect(m_checkoutRemoteButton, &QPushButton::clicked,
            this, &BranchManagerDialog::checkoutSelectedRemoteBranch);

    connect(m_createBranchButton, &QPushButton::clicked,
            this, &BranchManagerDialog::createNewBranch);

    connect(m_newBranchLineEdit, &QLineEdit::textChanged,
            this, [this]() {
                updateUiState();
            });

    connect(m_newBranchLineEdit, &QLineEdit::returnPressed,
            this, [this]() {
                if (m_createBranchButton != nullptr && m_createBranchButton->isEnabled()) {
                    createNewBranch();
                }
            });

    connect(m_localBranchesListWidget, &QListWidget::itemSelectionChanged,
            this, [this]() {
                if (m_localBranchesListWidget != nullptr
                    && m_localBranchesListWidget->currentItem() != nullptr
                    && m_remoteBranchesListWidget != nullptr) {
                    m_remoteBranchesListWidget->clearSelection();
                }

                updateUiState();
            });

    connect(m_remoteBranchesListWidget, &QListWidget::itemSelectionChanged,
            this, [this]() {
                if (m_remoteBranchesListWidget != nullptr
                    && m_remoteBranchesListWidget->currentItem() != nullptr
                    && m_localBranchesListWidget != nullptr) {
                    m_localBranchesListWidget->clearSelection();
                }

                updateUiState();
            });

    connect(m_localBranchesListWidget, &QListWidget::itemDoubleClicked,
            this, [this]() {
                checkoutSelectedLocalBranch();
            });

    connect(m_remoteBranchesListWidget, &QListWidget::itemDoubleClicked,
            this, [this]() {
                checkoutSelectedRemoteBranch();
            });

    if (m_commandConsoleWidget != nullptr) {
        connect(m_commandConsoleWidget, &CommandConsoleWidget::repositoryStateChanged,
                this, [this]() {
                    refreshBranches();

                    const QString currentBranch =
                        GitRepositoryInfo::currentBranch(m_repositoryPath);
                    emit branchChanged(currentBranch);
                });
    }
}

/***************************************************************************/
/*                                                                         */
/*                         Update Repository Info                          */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::updateRepositoryInfo()
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

    if (m_currentBranchLabel != nullptr) {
        const QString currentBranch = GitRepositoryInfo::currentBranch(m_repositoryPath);

        if (currentBranch.trimmed().isEmpty()) {
            m_currentBranchLabel->setText(tr("Current Branch: -"));
        } else {
            m_currentBranchLabel->setText(tr("Current Branch: %1").arg(currentBranch));
        }
    }
}

/***************************************************************************/
/*                                                                         */
/*                           Update UI State                               */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::updateUiState()
{
    const bool hasRepository = !m_repositoryPath.isEmpty();
    const bool hasLocalSelection = !selectedLocalBranch().isEmpty();
    const bool hasRemoteSelection = !selectedRemoteBranch().isEmpty();
    const bool hasNewBranchName = !m_newBranchLineEdit->text().trimmed().isEmpty();

    if (m_refreshButton != nullptr) {
        m_refreshButton->setEnabled(hasRepository);
    }

    if (m_checkoutLocalButton != nullptr) {
        m_checkoutLocalButton->setEnabled(hasRepository && hasLocalSelection);
    }

    if (m_checkoutRemoteButton != nullptr) {
        m_checkoutRemoteButton->setEnabled(hasRepository && hasRemoteSelection);
    }

    if (m_createBranchButton != nullptr) {
        m_createBranchButton->setEnabled(hasRepository && hasNewBranchName);
    }

    if (m_closeButton != nullptr) {
        m_closeButton->setEnabled(true);
    }

    if (m_commandConsoleWidget != nullptr) {
        m_commandConsoleWidget->setEnabled(hasRepository);
    }
}

/***************************************************************************/
/*                                                                         */
/*                            UI Helpers                                   */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::loadBranchesIntoUi(const GitBranchResult& result)
{
    if (m_localBranchesListWidget != nullptr) {
        m_localBranchesListWidget->clear();
    }

    if (m_remoteBranchesListWidget != nullptr) {
        m_remoteBranchesListWidget->clear();
    }

    for (const GitBranchEntry& entry : result.localBranches) {
        const QString text = entry.isCurrent
                                 ? QStringLiteral("* %1").arg(entry.name)
                                 : entry.name;

        auto* item = new QListWidgetItem(text, m_localBranchesListWidget);
        item->setData(Qt::UserRole, entry.name);
    }

    for (const GitBranchEntry& entry : result.remoteBranches) {
        auto* item = new QListWidgetItem(entry.fullName, m_remoteBranchesListWidget);
        item->setData(Qt::UserRole, entry.fullName);
    }
}

void BranchManagerDialog::selectCurrentLocalBranch()
{
    if (m_localBranchesListWidget == nullptr) {
        return;
    }

    const QString currentBranch = GitRepositoryInfo::currentBranch(m_repositoryPath).trimmed();
    if (currentBranch.isEmpty()) {
        return;
    }

    for (int i = 0; i < m_localBranchesListWidget->count(); ++i) {
        QListWidgetItem* item = m_localBranchesListWidget->item(i);
        if (item == nullptr) {
            continue;
        }

        const QString branchName = item->data(Qt::UserRole).toString().trimmed();
        if (branchName == currentBranch) {
            m_localBranchesListWidget->setCurrentItem(item);
            break;
        }
    }
}

QString BranchManagerDialog::selectedLocalBranch() const
{
    if (m_localBranchesListWidget == nullptr || m_localBranchesListWidget->currentItem() == nullptr) {
        return QString();
    }

    return m_localBranchesListWidget->currentItem()->data(Qt::UserRole).toString().trimmed();
}

QString BranchManagerDialog::selectedRemoteBranch() const
{
    if (m_remoteBranchesListWidget == nullptr || m_remoteBranchesListWidget->currentItem() == nullptr) {
        return QString();
    }

    return m_remoteBranchesListWidget->currentItem()->data(Qt::UserRole).toString().trimmed();
}

/***************************************************************************/
/*                                                                         */
/*                         Branch Actions                                  */
/*                                                                         */
/***************************************************************************/

void BranchManagerDialog::checkoutSelectedLocalBranch()
{
    const QString branchName = selectedLocalBranch();
    if (branchName.isEmpty()) {
        return;
    }

    GitCommandCheckout gitCommandCheckout(m_repositoryPath);
    gitCommandCheckout.setBranchName(branchName);
    gitCommandCheckout.setCreateTrackingBranch(false);

    const GitCheckoutResult result = gitCommandCheckout.execute();

    if (!result.success) {
        QMessageBox::critical(
            this,
            tr("Checkout Failed"),
            result.errorMessage.isEmpty()
                ? tr("The branch could not be checked out.")
                : result.errorMessage
            );
        return;
    }

    handleSuccessfulBranchChange(result.outputMessage);
}

void BranchManagerDialog::checkoutSelectedRemoteBranch()
{
    const QString fullRemoteRef = selectedRemoteBranch();
    if (fullRemoteRef.isEmpty()) {
        return;
    }

    const int separatorIndex = fullRemoteRef.indexOf('/');
    if (separatorIndex <= 0 || separatorIndex >= fullRemoteRef.length() - 1) {
        QMessageBox::warning(
            this,
            tr("Invalid Remote Branch"),
            tr("The selected remote branch reference is invalid.")
            );
        return;
    }

    const QString remoteName = fullRemoteRef.left(separatorIndex).trimmed();
    const QString branchName = fullRemoteRef.mid(separatorIndex + 1).trimmed();

    if (remoteName.isEmpty() || branchName.isEmpty()) {
        QMessageBox::warning(
            this,
            tr("Invalid Remote Branch"),
            tr("The selected remote branch reference is invalid.")
            );
        return;
    }

    GitCommandCheckout gitCommandCheckout(m_repositoryPath);
    gitCommandCheckout.setRemoteName(remoteName);
    gitCommandCheckout.setBranchName(branchName);
    gitCommandCheckout.setCreateTrackingBranch(true);

    const GitCheckoutResult result = gitCommandCheckout.execute();

    if (!result.success) {
        QMessageBox::critical(
            this,
            tr("Checkout Failed"),
            result.errorMessage.isEmpty()
                ? tr("The remote tracking branch could not be checked out.")
                : result.errorMessage
            );
        return;
    }

    handleSuccessfulBranchChange(result.outputMessage);
}

void BranchManagerDialog::createNewBranch()
{
    const QString branchName = m_newBranchLineEdit->text().trimmed();
    if (branchName.isEmpty()) {
        return;
    }

    GitCommandBranch gitCommandBranch(m_repositoryPath);
    const GitBranchCreateResult result =
        gitCommandBranch.createBranch(branchName, true);

    if (!result.success) {
        QMessageBox::critical(
            this,
            tr("Branch Creation Failed"),
            result.errorMessage.isEmpty()
                ? tr("The branch could not be created.")
                : result.errorMessage
            );
        return;
    }

    m_newBranchLineEdit->clear();
    handleSuccessfulBranchChange(result.outputMessage);
}

void BranchManagerDialog::handleSuccessfulBranchChange(const QString& outputMessage)
{
    refreshBranches();

    const QString currentBranch = GitRepositoryInfo::currentBranch(m_repositoryPath);
    emit branchChanged(currentBranch);

    if (!outputMessage.trimmed().isEmpty()) {
        QMessageBox::information(
            this,
            tr("Success"),
            outputMessage
            );
    }
}
