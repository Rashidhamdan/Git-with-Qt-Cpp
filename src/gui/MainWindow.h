#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QDockWidget>
#include <QTabWidget>

// Core UI
#include "RepoWidget.h"

// Dialogs / Widgets
#include "CommitDialog.h"
#include "InitRepoDialog.h"
#include "BranchManagerDialog.h"
#include "StashDialog.h"
#include "CommandConsoleWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, QString version = "");
    ~MainWindow();

private:
    Ui::MainWindow *ui = nullptr;

    QString m_version;

    /***************************************************************************/
    /*                                                                         */
    /*                         Repository State                                */
    /*                                                                         */
    /***************************************************************************/

    RepoWidget* m_repoWidget = nullptr;
    QString m_currentRepoPath;

    QString m_currentBranch;
    QStringList m_currentRemotes;
    QString m_currentUpstream;

    /***************************************************************************/
    /*                                                                         */
    /*                              Actions                                    */
    /*                                                                         */
    /***************************************************************************/

    // File
    QAction* m_actionNewRepo = nullptr;
    QAction* m_actionOpenRepo = nullptr;
    QAction* m_actionCloseRepo = nullptr;
    QAction* m_actionExit = nullptr;

    // Git
    QAction* m_actionStatus = nullptr;
    QAction* m_actionAdd = nullptr;
    QAction* m_actionCommit = nullptr;
    QAction* m_actionBranches = nullptr;
    QAction* m_actionPush = nullptr;
    QAction* m_actionPull = nullptr;
    QAction* m_actionDiff = nullptr;
    QAction* m_actionLog = nullptr;

    // View
    QAction* m_actionToggleConsole = nullptr;
    QAction* m_actionFocusConsole = nullptr;

    // Help
    QAction* m_actionAbout = nullptr;

    /***************************************************************************/
    /*                                                                         */
    /*                              Dialogs                                    */
    /*                                                                         */
    /***************************************************************************/

    CommitDialog* m_commitDialog = nullptr;
    InitRepoDialog* m_initRepoDialog = nullptr;
    BranchManagerDialog* m_branchManagerDialog = nullptr;
    StashDialog* m_stashDialog = nullptr;

    /***************************************************************************/
    /*                                                                         */
    /*                           Global Console                                */
    /*                                                                         */
    /***************************************************************************/

    QDockWidget* m_commandConsoleDock = nullptr;
    CommandConsoleWidget* m_commandConsoleWidget = nullptr;

    /***************************************************************************/
    /*                                                                         */
    /*                           Status Bar Widgets                            */
    /*                                                                         */
    /***************************************************************************/

    QLabel* m_repoInfoLabel = nullptr;

    /***************************************************************************/
    /*                                                                         */
    /*                         UI Initialization                               */
    /*                                                                         */
    /***************************************************************************/

    void createCentralUi();
    void createMenuBar(QMenuBar* menuBar);
    void createToolBar();
    void createStatusBar();
    void createWindowsAndWidgets();

    /***************************************************************************/
    /*                                                                         */
    /*                         Setup & State                                   */
    /*                                                                         */
    /***************************************************************************/

    void setupActions();
    void setupConnections();
    void updateUiState();
    void applyActionToolTip(QAction* action);

    /***************************************************************************/
    /*                                                                         */
    /*                         Repository Info                                 */
    /*                                                                         */
    /***************************************************************************/

    void refreshRepositoryInfo();
    void clearRepositoryInfo();
    void updateRepositoryInfoLabel();

    bool hasOpenRepository() const;
    bool hasUpstream() const;

    /***************************************************************************/
    /*                                                                         */
    /*                         Repository Handling                             */
    /*                                                                         */
    /***************************************************************************/

    void openRepository();
    void openRepositoryFromPath(const QString& path);
    void closeRepository();

    /***************************************************************************/
    /*                                                                         */
    /*                              View Helpers                               */
    /*                                                                         */
    /***************************************************************************/

    QTabWidget* findRepoTabWidget() const;
    void showStatusView();
    void showLogView();
    void showDiffView();
    void focusCommandConsole();

    /***************************************************************************/
    /*                                                                         */
    /*                              Dialogs                                    */
    /*                                                                         */
    /***************************************************************************/

    void showAboutDialog();
    void openInitRepoDialog();
    void openCommitDialog();
    void openBranchManagerDialog();
    void stageAllChanges();
    void pushChanges();
    void pullChanges();
};

#endif // MAINWINDOW_H
