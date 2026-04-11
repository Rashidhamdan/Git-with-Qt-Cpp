#ifndef REPOEXPLORERWIDGET_H
#define REPOEXPLORERWIDGET_H

#include <QWidget>
#include <QString>

class QLabel;
class QPushButton;
class QTreeView;
class QFileSystemModel;
class QVBoxLayout;

class RepoExplorerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RepoExplorerWidget(QWidget *parent = nullptr);
    ~RepoExplorerWidget();

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    void refresh();
    QString selectedFilePath() const;

signals:
    void fileActivated(const QString& filePath);
    void selectionChanged(const QString& filePath);
    void refreshRequested();
    void fileSelected(const QString& filePath);

private:
    QString m_repositoryPath;

    QLabel* m_repositoryLabel = nullptr;
    QPushButton* m_refreshButton = nullptr;

    QTreeView* m_treeView = nullptr;
    QFileSystemModel* m_fileSystemModel = nullptr;

    QVBoxLayout* m_mainLayout = nullptr;

private:
    void initializeUi();
    void createWidgets();
    void createLayouts();
    void setupConnections();
    void updateRepositoryInfo();
    void configureTreeView();
};

#endif // REPOEXPLORERWIDGET_H
