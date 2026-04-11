#ifndef BRANCHMANAGERDIALOG_H
#define BRANCHMANAGERDIALOG_H

#include <QDialog>
#include <QString>

#include "CommandConsoleWidget.h"
#include "GitCommandBranch.h"
#include "GitCommandCheckout.h"

class QLabel;
class QListWidget;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

/***************************************************************************/
/*                                                                         */
/*                     BranchManagerDialog                                 */
/*                                                                         */
/***************************************************************************/

class BranchManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BranchManagerDialog(QWidget *parent = nullptr);
    ~BranchManagerDialog();

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    void refreshBranches();

signals:
    void branchChanged(const QString& branchName);

private:
    QString m_repositoryPath;

    QLabel* m_repositoryLabel = nullptr;
    QLabel* m_currentBranchLabel = nullptr;

    QTabWidget* m_tabWidget = nullptr;

    QWidget* m_branchPage = nullptr;
    QListWidget* m_localBranchesListWidget = nullptr;
    QListWidget* m_remoteBranchesListWidget = nullptr;

    QLineEdit* m_newBranchLineEdit = nullptr;

    QPushButton* m_refreshButton = nullptr;
    QPushButton* m_checkoutLocalButton = nullptr;
    QPushButton* m_checkoutRemoteButton = nullptr;
    QPushButton* m_createBranchButton = nullptr;
    QPushButton* m_closeButton = nullptr;

    CommandConsoleWidget* m_commandConsoleWidget = nullptr;

    QVBoxLayout* m_mainLayout = nullptr;

private:
    void initializeUi();
    void createWidgets();
    void createLayouts();
    void setupConnections();
    void updateRepositoryInfo();
    void updateUiState();

    void loadBranchesIntoUi(const GitBranchResult& result);
    void selectCurrentLocalBranch();

    QString selectedLocalBranch() const;
    QString selectedRemoteBranch() const;

    void checkoutSelectedLocalBranch();
    void checkoutSelectedRemoteBranch();
    void createNewBranch();
    void handleSuccessfulBranchChange(const QString& outputMessage);
};

#endif // BRANCHMANAGERDIALOG_H
