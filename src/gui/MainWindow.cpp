#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "GitRepositoryInfo.h"
#include "StatusWidget.h"
#include "LogViewerWidget.h"
#include "DiffViewerWidget.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QStatusBar>
#include <QToolBar>
#include <QWidget>
#include <QStyle>
#include <QTabWidget>
/***************************************************************************/
/*                                                                         */
/*                             MainWindow                                  */
/*                                                                         */
/***************************************************************************/

MainWindow::MainWindow(QWidget *parent, QString version)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_version(version)
{
    ui->setupUi(this);

    createCentralUi();
    createWindowsAndWidgets();
    createMenuBar(new QMenuBar(this));
    setupActions();
    createToolBar();
    createStatusBar();
    setupConnections();
    clearRepositoryInfo();
    updateUiState();
}

/***************************************************************************/
/*                                                                         */
/*                             ~MainWindow                                 */
/*                                                                         */
/***************************************************************************/

MainWindow::~MainWindow()
{
    delete ui;
}

/***************************************************************************/
/*                                                                         */
/*                          createCentralUi                                */
/*                                                                         */
/***************************************************************************/

void MainWindow::createCentralUi()
{
    setCentralWidget(new QWidget(this));
}

/***************************************************************************/
/*                                                                         */
/*                           createMenuBar                                 */
/*                                                                         */
/***************************************************************************/

void MainWindow::createMenuBar(QMenuBar *menuBar)
{
    setMenuBar(menuBar);

    /************************ File ************************/

    QMenu *fileMenu = menuBar->addMenu(tr("File"));

    m_actionNewRepo   = new QAction(QIcon(":/icons/icons/create_new_folder.png"), tr("Create New Repository"), this);
    m_actionOpenRepo  = new QAction(QIcon(":/icons/icons/folder_open.png"), tr("Open Repository"), this);
    m_actionCloseRepo = new QAction(QIcon(":/icons/icons/close.png"), tr("Close Repository"), this);
    m_actionExit      = new QAction(QIcon(":/icons/icons/logout.png"), tr("Exit"), this);

    fileMenu->addAction(m_actionNewRepo);
    fileMenu->addAction(m_actionOpenRepo);
    fileMenu->addAction(m_actionCloseRepo);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actionExit);

    /************************ Git ************************/

    QMenu *gitMenu = menuBar->addMenu(tr("Git"));

    m_actionStatus   = new QAction(QIcon(":/icons/icons/info.png"), tr("Show Status"), this);
    m_actionAdd      = new QAction(QIcon(":/icons/icons/add_circle.png"), tr("Add Changes"), this);
    m_actionCommit   = new QAction(QIcon(":/icons/icons/task_alt.png"), tr("Commit"), this);
    m_actionBranches = new QAction(QIcon(":/icons/icons/branch.png"), tr("Branches"), this);
    m_actionDiff     = new QAction(QIcon(":/icons/icons/difference.png"), tr("Show Diff"), this);
    m_actionLog      = new QAction(QIcon(":/icons/icons/history.png"), tr("View Log"), this);

    gitMenu->addAction(m_actionStatus);
    gitMenu->addAction(m_actionAdd);
    gitMenu->addAction(m_actionCommit);
    gitMenu->addAction(m_actionBranches);
    gitMenu->addAction(m_actionDiff);
    gitMenu->addAction(m_actionLog);

    /************************ Remote ************************/

    QMenu *remoteMenu = menuBar->addMenu(tr("Remote"));

    m_actionPush = new QAction(QIcon(":/icons/icons/upload.png"), tr("Push"), this);
    m_actionPull = new QAction(QIcon(":/icons/icons/download.png"), tr("Pull"), this);

    remoteMenu->addAction(m_actionPush);
    remoteMenu->addAction(m_actionPull);

    /************************ View ************************/

    QMenu *viewMenu = menuBar->addMenu(tr("View"));

    if (m_actionToggleConsole != nullptr) {
        viewMenu->addAction(m_actionToggleConsole);
    }

    m_actionFocusConsole = new QAction(QIcon(":/icons/icons/terminal.png"), tr("Focus Console"), this);
    viewMenu->addAction(m_actionFocusConsole);

    /************************ Help ************************/

    QMenu *helpMenu = menuBar->addMenu(tr("Help"));

    m_actionAbout = new QAction(QIcon(":/icons/icons/info.png"), tr("About"), this);
    helpMenu->addAction(m_actionAbout);
}

/***************************************************************************/
/*                                                                         */
/*                             createToolBar                               */
/*                                                                         */
/***************************************************************************/

void MainWindow::createToolBar()
{
    QToolBar *mainToolBar = addToolBar(tr("Main Toolbar"));
    mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
    mainToolBar->setMovable(false);

    mainToolBar->addAction(m_actionOpenRepo);
    mainToolBar->addAction(m_actionCloseRepo);
    mainToolBar->addSeparator();
    mainToolBar->addAction(m_actionStatus);
    mainToolBar->addAction(m_actionAdd);
    mainToolBar->addAction(m_actionCommit);
    mainToolBar->addAction(m_actionBranches);
    mainToolBar->addAction(m_actionLog);
    mainToolBar->addAction(m_actionDiff);
    mainToolBar->addSeparator();
    mainToolBar->addAction(m_actionPush);
    mainToolBar->addAction(m_actionPull);
    mainToolBar->addSeparator();
    mainToolBar->addAction(m_actionFocusConsole);
}

/***************************************************************************/
/*                                                                         */
/*                           createStatusBar                               */
/*                                                                         */
/***************************************************************************/

void MainWindow::createStatusBar()
{
    m_repoInfoLabel = new QLabel(this);
    m_repoInfoLabel->setText(tr("No repository loaded"));

    statusBar()->showMessage(tr("Ready"));
    statusBar()->addPermanentWidget(m_repoInfoLabel, 1);
}

/***************************************************************************/
/*                                                                         */
/*                    createWindowsAndWidgets                              */
/*                                                                         */
/***************************************************************************/

void MainWindow::createWindowsAndWidgets()
{
    m_commitDialog = new CommitDialog(this);
    m_initRepoDialog = new InitRepoDialog(this);
    m_branchManagerDialog = new BranchManagerDialog(this);
    m_stashDialog = new StashDialog(this);

    m_commandConsoleWidget = new CommandConsoleWidget(this);

    m_commandConsoleDock = new QDockWidget(tr("Command Console"), this);
    m_commandConsoleDock->setObjectName(QStringLiteral("commandConsoleDock"));
    m_commandConsoleDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    m_commandConsoleDock->setWidget(m_commandConsoleWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_commandConsoleDock);

    m_actionToggleConsole = m_commandConsoleDock->toggleViewAction();
    m_actionToggleConsole->setIcon(QIcon(":/icons/icons/terminal.png"));
    m_actionToggleConsole->setText(tr("Command Console"));

    m_commandConsoleDock->hide();
}

/***************************************************************************/
/*                                                                         */
/*                             setupActions                                */
/*                                                                         */
/***************************************************************************/

void MainWindow::setupActions()
{
    m_actionNewRepo->setShortcut(QKeySequence::New);
    m_actionOpenRepo->setShortcut(QKeySequence::Open);
    m_actionCloseRepo->setShortcut(QKeySequence::Close);
    m_actionExit->setShortcut(QKeySequence::Quit);

    m_actionStatus->setShortcut(QKeySequence(tr("Ctrl+1")));
    m_actionLog->setShortcut(QKeySequence(tr("Ctrl+2")));
    m_actionDiff->setShortcut(QKeySequence(tr("Ctrl+3")));
    m_actionAdd->setShortcut(QKeySequence(tr("Ctrl+Shift+A")));
    m_actionCommit->setShortcut(QKeySequence(tr("Ctrl+Shift+C")));
    m_actionBranches->setShortcut(QKeySequence(tr("Ctrl+B")));
    m_actionPush->setShortcut(QKeySequence(tr("Ctrl+Shift+P")));
    m_actionPull->setShortcut(QKeySequence(tr("Ctrl+Shift+L")));

    m_actionOpenRepo->setStatusTip(tr("Open an existing repository"));
    m_actionCloseRepo->setStatusTip(tr("Close the currently opened repository"));
    m_actionExit->setStatusTip(tr("Exit the application"));

    m_actionStatus->setStatusTip(tr("Switch to the status view"));
    m_actionAdd->setStatusTip(tr("Stage all changes"));
    m_actionCommit->setStatusTip(tr("Create a commit"));
    m_actionBranches->setStatusTip(tr("Show and manage branches"));
    m_actionDiff->setStatusTip(tr("Switch to the diff view"));
    m_actionLog->setStatusTip(tr("Switch to the log view"));
    m_actionPush->setStatusTip(tr("Push commits to the configured upstream"));
    m_actionPull->setStatusTip(tr("Pull changes from the configured upstream"));

    if (m_actionToggleConsole != nullptr) {
        m_actionToggleConsole->setShortcut(QKeySequence(tr("Ctrl+Alt+T")));
        m_actionToggleConsole->setStatusTip(tr("Show or hide the global command console"));
    }

    if (m_actionFocusConsole != nullptr) {
        m_actionFocusConsole->setShortcut(QKeySequence(tr("Ctrl+Alt+C")));
        m_actionFocusConsole->setStatusTip(tr("Show and focus the command console, or hide it if it is already visible"));
    }

    m_actionAbout->setStatusTip(tr("Show application information"));

    applyActionToolTip(m_actionNewRepo);
    applyActionToolTip(m_actionOpenRepo);
    applyActionToolTip(m_actionCloseRepo);
    applyActionToolTip(m_actionExit);
    applyActionToolTip(m_actionStatus);
    applyActionToolTip(m_actionAdd);
    applyActionToolTip(m_actionCommit);
    applyActionToolTip(m_actionBranches);
    applyActionToolTip(m_actionLog);
    applyActionToolTip(m_actionDiff);
    applyActionToolTip(m_actionPush);
    applyActionToolTip(m_actionPull);
    applyActionToolTip(m_actionToggleConsole);
    applyActionToolTip(m_actionFocusConsole);
    applyActionToolTip(m_actionAbout);
}

/***************************************************************************/
/*                                                                         */
/*                         applyActionToolTip                              */
/*                                                                         */
/***************************************************************************/

void MainWindow::applyActionToolTip(QAction* action)
{

    if (action == nullptr) {
        return;
    }

    const QString shortcutText = action->shortcut().toString(QKeySequence::NativeText);

    if (shortcutText.isEmpty()) {
        action->setToolTip(action->text());
    } else {
        action->setToolTip(tr("%1 (%2)").arg(action->text(), shortcutText));
    }
}

/***************************************************************************/
/*                                                                         */
/*                          setupConnections                               */
/*                                                                         */
/***************************************************************************/

void MainWindow::setupConnections()
{
    connect(m_actionNewRepo, &QAction::triggered, this, &MainWindow::openInitRepoDialog);
    connect(m_actionOpenRepo, &QAction::triggered, this, &MainWindow::openRepository);
    connect(m_actionCloseRepo, &QAction::triggered, this, &MainWindow::closeRepository);
    connect(m_actionExit, &QAction::triggered, this, &MainWindow::close);

    connect(m_actionStatus, &QAction::triggered, this, &MainWindow::showStatusView);
    connect(m_actionAdd, &QAction::triggered, this, &MainWindow::stageAllChanges);
    connect(m_actionCommit, &QAction::triggered, this, &MainWindow::openCommitDialog);
    connect(m_actionBranches, &QAction::triggered, this, &MainWindow::openBranchManagerDialog);
    connect(m_actionLog, &QAction::triggered, this, &MainWindow::showLogView);
    connect(m_actionDiff, &QAction::triggered, this, &MainWindow::showDiffView);

    connect(m_actionPush, &QAction::triggered, this, &MainWindow::pushChanges);
    connect(m_actionPull, &QAction::triggered, this, &MainWindow::pullChanges);

    connect(m_actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(m_actionFocusConsole, &QAction::triggered, this, &MainWindow::focusCommandConsole);

    connect(m_branchManagerDialog, &BranchManagerDialog::branchChanged,
            this, [this](const QString&) {
                if (m_repoWidget != nullptr) {
                    m_repoWidget->refreshUi();
                }

                if (m_commandConsoleWidget != nullptr) {
                    m_commandConsoleWidget->setRepositoryPath(m_currentRepoPath);
                }

                refreshRepositoryInfo();
                updateUiState();
            });

    if (m_commandConsoleWidget != nullptr) {
        connect(m_commandConsoleWidget, &CommandConsoleWidget::repositoryStateChanged,
                this, [this]() {
                    if (m_repoWidget != nullptr) {
                        m_repoWidget->refreshUi();
                    }

                    refreshRepositoryInfo();
                    updateUiState();
                });
    }
}

/***************************************************************************/
/*                                                                         */
/*                           hasOpenRepository                             */
/*                                                                         */
/***************************************************************************/

bool MainWindow::hasOpenRepository() const
{
    return (m_repoWidget != nullptr) && !m_currentRepoPath.trimmed().isEmpty();
}

/***************************************************************************/
/*                                                                         */
/*                              hasUpstream                                */
/*                                                                         */
/***************************************************************************/

bool MainWindow::hasUpstream() const
{
    return !m_currentUpstream.trimmed().isEmpty();
}

/***************************************************************************/
/*                                                                         */
/*                         clearRepositoryInfo                             */
/*                                                                         */
/***************************************************************************/

void MainWindow::clearRepositoryInfo()
{
    m_currentBranch.clear();
    m_currentRemotes.clear();
    m_currentUpstream.clear();

    updateRepositoryInfoLabel();
}

/***************************************************************************/
/*                                                                         */
/*                        refreshRepositoryInfo                            */
/*                                                                         */
/***************************************************************************/

void MainWindow::refreshRepositoryInfo()
{
    clearRepositoryInfo();

    if (!hasOpenRepository()) {
        return;
    }

    m_currentBranch = GitRepositoryInfo::currentBranch(m_currentRepoPath);
    m_currentRemotes = GitRepositoryInfo::remotes(m_currentRepoPath);
    m_currentUpstream = GitRepositoryInfo::upstream(m_currentRepoPath);

    updateRepositoryInfoLabel();
}

/***************************************************************************/
/*                                                                         */
/*                      updateRepositoryInfoLabel                          */
/*                                                                         */
/***************************************************************************/

void MainWindow::updateRepositoryInfoLabel()
{
    if (m_repoInfoLabel == nullptr) {
        return;
    }

    if (!hasOpenRepository()) {
        m_repoInfoLabel->setText(tr("No repository loaded"));
        m_repoInfoLabel->setToolTip(QString());
        return;
    }

    const QString repoName = QFileInfo(m_currentRepoPath).fileName();
    const QString branchText = m_currentBranch.isEmpty() ? tr("-") : m_currentBranch;
    const QString remotesText = m_currentRemotes.isEmpty() ? tr("-") : m_currentRemotes.join(", ");
    const QString upstreamText = m_currentUpstream.isEmpty() ? tr("-") : m_currentUpstream;

    m_repoInfoLabel->setText(
        tr("Repo: %1 | Branch: %2 | Remotes: %3 | Upstream: %4")
            .arg(repoName, branchText, remotesText, upstreamText)
        );
    m_repoInfoLabel->setToolTip(m_currentRepoPath);
}

/***************************************************************************/
/*                                                                         */
/*                             updateUiState                               */
/*                                                                         */
/***************************************************************************/

void MainWindow::updateUiState()
{
    const bool repositoryOpen = hasOpenRepository();
    const bool stagedChangesAvailable =
        (m_repoWidget != nullptr && m_repoWidget->hasStagedChanges());
    const bool upstreamAvailable = repositoryOpen && hasUpstream();

    m_actionNewRepo->setEnabled(true);
    m_actionOpenRepo->setEnabled(true);
    m_actionCloseRepo->setEnabled(repositoryOpen);
    m_actionExit->setEnabled(true);

    m_actionStatus->setEnabled(repositoryOpen);
    m_actionAdd->setEnabled(repositoryOpen);
    m_actionCommit->setEnabled(stagedChangesAvailable);
    m_actionBranches->setEnabled(repositoryOpen);
    m_actionPush->setEnabled(upstreamAvailable);
    m_actionPull->setEnabled(upstreamAvailable);
    m_actionDiff->setEnabled(repositoryOpen);
    m_actionLog->setEnabled(repositoryOpen);

    if (m_commandConsoleWidget != nullptr) {
        m_commandConsoleWidget->setRepositoryPath(repositoryOpen ? m_currentRepoPath : QString());
    }

    if (m_actionToggleConsole != nullptr) {
        m_actionToggleConsole->setEnabled(true);
    }

    if (m_actionFocusConsole != nullptr) {
        m_actionFocusConsole->setEnabled(true);
    }

    updateRepositoryInfoLabel();

    if (repositoryOpen) {
        const QString repoName = QFileInfo(m_currentRepoPath).fileName();
        statusBar()->showMessage(tr("Repository loaded: %1").arg(repoName));
    } else {
        statusBar()->showMessage(tr("No repository loaded"));
    }
}

/***************************************************************************/
/*                                                                         */
/*                          openRepository                                 */
/*                                                                         */
/***************************************************************************/

void MainWindow::openRepository()
{
    const QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Open Repository"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (dir.isEmpty()) {
        return;
    }

    openRepositoryFromPath(dir);
}

/***************************************************************************/
/*                                                                         */
/*                       openRepositoryFromPath                            */
/*                                                                         */
/***************************************************************************/

void MainWindow::openRepositoryFromPath(const QString &path)
{
    const QString repositoryRoot = GitRepositoryInfo::repositoryRoot(path);

    if (repositoryRoot.trimmed().isEmpty()) {
        QMessageBox::warning(
            this,
            tr("Invalid Repository"),
            tr("The selected folder is not a valid Git repository.")
            );
        return;
    }

    QWidget* oldCentralWidget = takeCentralWidget();
    if (oldCentralWidget != nullptr) {
        delete oldCentralWidget;
    }

    m_repoWidget = new RepoWidget(repositoryRoot, this);

    connect(m_repoWidget, &RepoWidget::repositoryStateChanged, this, [this]() {
        refreshRepositoryInfo();
        updateUiState();
    });

    connect(m_repoWidget, &RepoWidget::commitRequestedFromUi, this, &MainWindow::openCommitDialog);
    connect(m_repoWidget, &RepoWidget::pushRequestedFromUi, this, &MainWindow::pushChanges);
    connect(m_repoWidget, &RepoWidget::pullRequestedFromUi, this, &MainWindow::pullChanges);

    setCentralWidget(m_repoWidget);

    m_currentRepoPath = repositoryRoot;

    if (m_commandConsoleWidget != nullptr) {
        m_commandConsoleWidget->setRepositoryPath(repositoryRoot);
    }

    if (m_branchManagerDialog != nullptr) {
        m_branchManagerDialog->setRepositoryPath(repositoryRoot);
    }

    refreshRepositoryInfo();
    updateUiState();
}

/***************************************************************************/
/*                                                                         */
/*                         closeRepository                                 */
/*                                                                         */
/***************************************************************************/

void MainWindow::closeRepository()
{
    if (m_repoWidget == nullptr) {
        return;
    }

    const QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Close Repository"),
        tr("Do you want to close the current repository?"),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply != QMessageBox::Yes) {
        return;
    }

    QWidget* oldCentralWidget = takeCentralWidget();
    if (oldCentralWidget != nullptr) {
        delete oldCentralWidget;
    }

    setCentralWidget(new QWidget(this));

    m_repoWidget = nullptr;
    m_currentRepoPath.clear();

    if (m_commandConsoleWidget != nullptr) {
        m_commandConsoleWidget->setRepositoryPath(QString());
    }

    if (m_branchManagerDialog != nullptr) {
        m_branchManagerDialog->setRepositoryPath(QString());
    }

    clearRepositoryInfo();
    updateUiState();
}

/***************************************************************************/
/*                                                                         */
/*                           findRepoTabWidget                             */
/*                                                                         */
/***************************************************************************/

QTabWidget* MainWindow::findRepoTabWidget() const
{
    if (m_repoWidget == nullptr) {
        return nullptr;
    }

    return m_repoWidget->findChild<QTabWidget*>();
}

/***************************************************************************/
/*                                                                         */
/*                            showStatusView                               */
/*                                                                         */
/***************************************************************************/

void MainWindow::showStatusView()
{
    if (m_repoWidget == nullptr) {
        return;
    }

    QTabWidget* tabWidget = findRepoTabWidget();
    StatusWidget* statusWidget = m_repoWidget->findChild<StatusWidget*>();

    if (tabWidget != nullptr && statusWidget != nullptr) {
        tabWidget->setCurrentWidget(statusWidget);
        statusWidget->setFocus(Qt::ShortcutFocusReason);
    }
}

/***************************************************************************/
/*                                                                         */
/*                              showLogView                                */
/*                                                                         */
/***************************************************************************/

void MainWindow::showLogView()
{
    if (m_repoWidget == nullptr) {
        return;
    }

    QTabWidget* tabWidget = findRepoTabWidget();
    LogViewerWidget* logWidget = m_repoWidget->findChild<LogViewerWidget*>();

    if (tabWidget != nullptr && logWidget != nullptr) {
        tabWidget->setCurrentWidget(logWidget);
        logWidget->setFocus(Qt::ShortcutFocusReason);
    }
}

/***************************************************************************/
/*                                                                         */
/*                              showDiffView                               */
/*                                                                         */
/***************************************************************************/

void MainWindow::showDiffView()
{
    if (m_repoWidget == nullptr) {
        return;
    }

    QTabWidget* tabWidget = findRepoTabWidget();
    DiffViewerWidget* diffWidget = m_repoWidget->findChild<DiffViewerWidget*>();

    if (tabWidget != nullptr && diffWidget != nullptr) {
        tabWidget->setCurrentWidget(diffWidget);
        diffWidget->setFocus(Qt::ShortcutFocusReason);
    }
}

/***************************************************************************/
/*                                                                         */
/*                         focusCommandConsole                             */
/*                                                                         */
/***************************************************************************/

void MainWindow::focusCommandConsole()
{
    if (m_commandConsoleDock == nullptr || m_commandConsoleWidget == nullptr) {
        return;
    }

    if (m_commandConsoleDock->isVisible()) {
        m_commandConsoleDock->hide();
        return;
    }

    m_commandConsoleDock->show();
    m_commandConsoleDock->raise();
    m_commandConsoleWidget->setFocus(Qt::ShortcutFocusReason);
}

/***************************************************************************/
/*                                                                         */
/*                             showAboutDialog                             */
/*                                                                         */
/***************************************************************************/

void MainWindow::showAboutDialog()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle(tr("About Git GUI"));
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setStandardButtons(QMessageBox::Ok);

    const QString htmlText = QString(
                                 "<div style='min-width: 400px;'>"
                                 "<h2 style='margin-bottom: 2px;'>Git GUI</h2>"
                                 "<p style='margin-top: 0px;'>Version %1</p>"
                                 "<p style='margin-top: 10px;'>A simple Git GUI developed with Qt/C++.</p>"
                                 "<p style='margin-top: 15px;'>"
                                 "<b>Author:</b> Rashid Hamdan<br>"
                                 "<b>E-Mail:</b> <a href='mailto:rashid.hamdan@outlook.de'>rashid.hamdan@outlook.de</a><br>"
                                 "<b>LinkedIn:</b> <a href='https://www.linkedin.com/in/rashid-hamdan-0307ab267'>rashid-hamdan-0307ab267</a><br>"
                                 "<b>XING:</b> <a href='https://www.xing.com/profile/Rashid_Hamdan086896'>Rashid_Hamdan086896</a><br>"
                                 "<b>GitHub:</b> <a href='https://github.com/Rashidhamdan'>github.com/Rashidhamdan</a>"
                                 "</p>"
                                 "</div>"
                                 ).arg(m_version);

    aboutBox.setText(htmlText);
    aboutBox.setIconPixmap(
        QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(96, 96)
        );

    aboutBox.exec();
}

/***************************************************************************/
/*                                                                         */
/*                         openInitRepoDialog                              */
/*                                                                         */
/***************************************************************************/

void MainWindow::openInitRepoDialog()
{
    QMessageBox::information(
        this,
        tr("Create New Repository"),
        tr("The repository creation dialog is not implemented yet.\n\nUse the command console or create a repository manually and open it afterwards.")
        );

    focusCommandConsole();
}

/***************************************************************************/
/*                                                                         */
/*                           openCommitDialog                              */
/*                                                                         */
/***************************************************************************/

void MainWindow::openCommitDialog()
{
    if (m_repoWidget == nullptr) {
        QMessageBox::information(
            this,
            tr("No Repository"),
            tr("Please open a repository first.")
            );
        return;
    }

    if (m_commitDialog == nullptr) {
        m_commitDialog = new CommitDialog(this);
    }

    m_commitDialog->setRepositoryPath(m_currentRepoPath);
    m_commitDialog->clearForm();

    if (m_commitDialog->exec() != QDialog::Accepted) {
        return;
    }

    const QString message = m_commitDialog->commitMessage().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(
            this,
            tr("Empty Commit Message"),
            tr("Please enter a commit message.")
            );
        return;
    }

    QString errorMessage;
    QString commitHash;

    const bool success = m_repoWidget->createCommit(message, &errorMessage, &commitHash);

    if (!success) {
        QMessageBox::critical(
            this,
            tr("Commit Failed"),
            errorMessage.isEmpty()
                ? tr("The commit could not be created.")
                : errorMessage
            );
        return;
    }

    refreshRepositoryInfo();
    updateUiState();

    QMessageBox::information(
        this,
        tr("Commit Created"),
        commitHash.isEmpty()
            ? tr("The commit was created successfully.")
            : tr("The commit was created successfully.\n\nCommit: %1").arg(commitHash)
        );

    statusBar()->showMessage(tr("Commit created successfully."), 3000);
}

/***************************************************************************/
/*                                                                         */
/*                        openBranchManagerDialog                          */
/*                                                                         */
/***************************************************************************/

void MainWindow::openBranchManagerDialog()
{
    if (m_repoWidget == nullptr) {
        QMessageBox::information(
            this,
            tr("No Repository"),
            tr("Please open a repository first.")
            );
        return;
    }

    if (m_branchManagerDialog == nullptr) {
        m_branchManagerDialog = new BranchManagerDialog(this);
    }

    m_branchManagerDialog->setRepositoryPath(m_currentRepoPath);
    m_branchManagerDialog->refreshBranches();
    m_branchManagerDialog->exec();
}

/***************************************************************************/
/*                                                                         */
/*                           stageAllChanges                               */
/*                                                                         */
/***************************************************************************/

void MainWindow::stageAllChanges()
{
    if (m_repoWidget == nullptr) {
        QMessageBox::information(
            this,
            tr("No Repository"),
            tr("Please open a repository first.")
            );
        return;
    }

    const bool success = m_repoWidget->stageAllChanges();

    if (!success) {
        QMessageBox::critical(
            this,
            tr("Git Add Failed"),
            tr("The changes could not be staged.")
            );
        return;
    }

    refreshRepositoryInfo();
    updateUiState();

    statusBar()->showMessage(tr("Changes staged successfully."), 3000);
}

/***************************************************************************/
/*                                                                         */
/*                             pushChanges                                 */
/*                                                                         */
/***************************************************************************/

void MainWindow::pushChanges()
{
    if (m_repoWidget == nullptr) {
        QMessageBox::information(
            this,
            tr("No Repository"),
            tr("Please open a repository first.")
            );
        return;
    }

    if (!hasUpstream()) {
        QMessageBox::warning(
            this,
            tr("No Upstream Configured"),
            tr("The current branch has no upstream configured.\n\nPlease configure a remote tracking branch first.")
            );
        return;
    }

    QString errorMessage;
    QString outputMessage;

    const bool success = m_repoWidget->pushChanges(&errorMessage, &outputMessage);

    if (!success) {
        QMessageBox::critical(
            this,
            tr("Push Failed"),
            errorMessage.isEmpty()
                ? tr("The push operation failed.")
                : errorMessage
            );
        return;
    }

    refreshRepositoryInfo();
    updateUiState();

    statusBar()->showMessage(
        tr("Push completed successfully for upstream %1.").arg(m_currentUpstream),
        4000
        );

    if (!outputMessage.trimmed().isEmpty()) {
        QMessageBox::information(
            this,
            tr("Push Successful"),
            outputMessage
            );
    }
}

/***************************************************************************/
/*                                                                         */
/*                             pullChanges                                 */
/*                                                                         */
/***************************************************************************/

void MainWindow::pullChanges()
{
    if (m_repoWidget == nullptr) {
        QMessageBox::information(
            this,
            tr("No Repository"),
            tr("Please open a repository first.")
            );
        return;
    }

    if (!hasUpstream()) {
        QMessageBox::warning(
            this,
            tr("No Upstream Configured"),
            tr("The current branch has no upstream configured.\n\nPlease configure a remote tracking branch first.")
            );
        return;
    }

    QString errorMessage;
    QString outputMessage;

    const bool success = m_repoWidget->pullChanges(&errorMessage, &outputMessage);

    if (!success) {
        QMessageBox::critical(
            this,
            tr("Pull Failed"),
            errorMessage.isEmpty()
                ? tr("The pull operation failed.")
                : errorMessage
            );
        return;
    }

    refreshRepositoryInfo();
    updateUiState();

    statusBar()->showMessage(
        tr("Pull completed successfully for upstream %1.").arg(m_currentUpstream),
        4000
        );

    if (!outputMessage.trimmed().isEmpty()) {
        QMessageBox::information(
            this,
            tr("Pull Successful"),
            outputMessage
            );
    }
}
