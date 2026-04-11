#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>
#include <QStringList>

class QLabel;
class QPushButton;
class QGroupBox;
class QListWidget;
class QVBoxLayout;

class StatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusWidget(QWidget *parent = nullptr);
    ~StatusWidget();

    void setRepositoryName(const QString& repositoryName);
    void setStagedFiles(const QStringList& files);
    void setModifiedFiles(const QStringList& files);
    void setUntrackedFiles(const QStringList& files);

    QStringList selectedFiles() const;
    bool hasStagedFiles() const;
    bool hasRepository() const;

    void clearStatus();
    void refreshView();

signals:
    void refreshRequested();
    void addSelectedRequested(const QStringList& filePaths);
    void commitRequested();
    void pushRequested();
    void pullRequested();

private:
    QLabel* m_repositoryLabel = nullptr;

    QPushButton* m_addSelectedButton = nullptr;
    QPushButton* m_commitButton = nullptr;
    QPushButton* m_pushButton = nullptr;
    QPushButton* m_pullButton = nullptr;
    QPushButton* m_refreshButton = nullptr;

    QGroupBox* m_stagedGroupBox = nullptr;
    QGroupBox* m_modifiedGroupBox = nullptr;
    QGroupBox* m_untrackedGroupBox = nullptr;

    QListWidget* m_stagedListWidget = nullptr;
    QListWidget* m_modifiedListWidget = nullptr;
    QListWidget* m_untrackedListWidget = nullptr;

    QVBoxLayout* m_mainLayout = nullptr;

private:
    void initializeUi();
    void createWidgets();
    void createLayouts();
    void setupConnections();
    void updateGroupTitles();
    void updateUiState();
};

#endif // STATUSWIDGET_H
