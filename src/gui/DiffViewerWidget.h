#ifndef DIFFVIEWERWIDGET_H
#define DIFFVIEWERWIDGET_H

#include <QWidget>
#include <QString>
#include <QRegularExpression>

class QLabel;
class QPushButton;
class QPlainTextEdit;
class QTextEdit;
class QVBoxLayout;
class QHBoxLayout;

class DiffViewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DiffViewerWidget(QWidget *parent = nullptr);
    ~DiffViewerWidget();

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    void setFilePath(const QString& filePath);
    QString filePath() const;

    void setDiffText(const QString& diffText);
    QString diffText() const;

    void clearViewer();
    void showPlaceholderMessage(const QString& message = QString());
    void refreshView();

signals:
    void refreshRequested();
    void filePathChanged(const QString& filePath);

private:
    QString m_repositoryPath;
    QString m_filePath;
    QString m_diffText;

    QLabel* m_repositoryLabel = nullptr;
    QLabel* m_filePathLabel = nullptr;
    QPushButton* m_refreshButton = nullptr;
    QTextEdit* m_diffTextEdit = nullptr;
    QVBoxLayout* m_mainLayout = nullptr;

private:
    void initializeUi();
    void createWidgets();
    void createLayouts();
    void setupConnections();
    void updateHeaderInfo();
};

#endif // DIFFVIEWERWIDGET_H
