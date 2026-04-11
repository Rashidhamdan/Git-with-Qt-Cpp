#ifndef LOGVIEWERWIDGET_H
#define LOGVIEWERWIDGET_H

#include <QWidget>
#include <QList>

#include "GitCommandLog.h"

class QLabel;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class QTextEdit;
class QSplitter;
class QVBoxLayout;

class LogViewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogViewerWidget(QWidget *parent = nullptr);
    ~LogViewerWidget();

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    void setLogEntries(const QList<GitLogEntry>& entries);
    QList<GitLogEntry> logEntries() const;

    void clearViewer();
    void refreshView();
    void showPlaceholderMessage(const QString& message = QString());

signals:
    void refreshRequested();
    void commitSelected(const QString& fullHash);

private:
    QString m_repositoryPath;
    QList<GitLogEntry> m_logEntries;

    QLabel* m_repositoryLabel = nullptr;
    QPushButton* m_refreshButton = nullptr;

    QSplitter* m_mainSplitter = nullptr;
    QListWidget* m_commitListWidget = nullptr;
    QTextEdit* m_commitDetailsTextEdit = nullptr;

    QVBoxLayout* m_mainLayout = nullptr;

private:
    void initializeUi();
    void createWidgets();
    void createLayouts();
    void setupConnections();
    void updateHeaderInfo();
    void updateCommitList();
    void showCommitDetails(int index);
};

#endif // LOGVIEWERWIDGET_H
