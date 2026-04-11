#ifndef REPOWIDGET_H
#define REPOWIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>

class QLabel;
class QPushButton;
class QSplitter;
class QTabWidget;
class QTreeView;
class QListWidget;
class QPlainTextEdit;
class QFileSystemModel;
class QWidget;
class QGroupBox;
class CommandConsoleWidget;

/***************************************************************************/
/*                                                                         */
/*                            RepoWidget                                   */
/*                                                                         */
/***************************************************************************/

class RepoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RepoWidget(const QString& repositoryPath = QString(),
                        QWidget* parent = nullptr);
    ~RepoWidget() override;

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    void refreshUi();

    bool hasStagedChanges() const;

    bool stageSelectedFiles(QString* errorMessage = nullptr);
    bool stageAllChanges(QString* errorMessage = nullptr);

    bool createCommit(const QString& message,
                      QString* errorMessage = nullptr,
                      QString* commitHash = nullptr);

    bool pushChanges(QString* errorMessage = nullptr,
                     QString* outputMessage = nullptr);

    bool pullChanges(QString* errorMessage = nullptr,
                     QString* outputMessage = nullptr);

signals:
    void repositoryStateChanged();
    void commitRequestedFromUi();
    void pushRequestedFromUi();
    void pullRequestedFromUi();

private:
    QString m_repositoryPath;

    QStringList m_stagedFiles;
    QStringList m_modifiedFiles;
    QStringList m_untrackedFiles;

    QLabel* m_repositoryLabel = nullptr;

    QPushButton* m_refreshButton = nullptr;
    QPushButton* m_addSelectedButton = nullptr;
    QPushButton* m_commitButton = nullptr;
    QPushButton* m_pushButton = nullptr;
    QPushButton* m_pullButton = nullptr;
    QPushButton* m_refreshStatusButton = nullptr;

    QSplitter* m_mainSplitter = nullptr;

    QFileSystemModel* m_fileSystemModel = nullptr;
    QTreeView* m_repoTreeView = nullptr;

    QTabWidget* m_tabWidget = nullptr;

    QWidget* m_statusPage = nullptr;
    QWidget* m_logPage = nullptr;
    QWidget* m_diffPage = nullptr;

    QListWidget* m_stagedListWidget = nullptr;
    QListWidget* m_modifiedListWidget = nullptr;
    QListWidget* m_untrackedListWidget = nullptr;

    QPlainTextEdit* m_logTextEdit = nullptr;
    QPlainTextEdit* m_diffTextEdit = nullptr;

    CommandConsoleWidget* m_commandConsoleWidget = nullptr;

private:
    void initializeUi();
    void createWidgets();
    void createLayouts();
    void setupConnections();

    void updateRepositoryLabel();
    void updateUiState();

    void refreshStatusView();
    void refreshLogView();
    void clearDiffView();
    void showDiffForPath(const QString& relativePath);

    QStringList selectedRepositoryPaths() const;
    QString currentDiffCandidatePath() const;
    QString toRepositoryRelativePath(const QString& absolutePath) const;

    static bool splitUpstreamReference(const QString& upstream,
                                       QString* remoteName,
                                       QString* branchName);
    static void setOptionalString(QString* target, const QString& value);
};

#endif // REPOWIDGET_H
