#ifndef COMMITDIALOG_H
#define COMMITDIALOG_H

#include <QDialog>
#include <QString>

class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class CommitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommitDialog(QWidget *parent = nullptr);
    ~CommitDialog();

    void setRepositoryPath(const QString& repositoryPath);
    QString repositoryPath() const;

    QString commitMessage() const;
    void clearForm();

private:
    QString m_repositoryPath;

    QLabel* m_repositoryLabel = nullptr;
    QLabel* m_summaryLabel = nullptr;
    QLabel* m_descriptionLabel = nullptr;

    QLineEdit* m_summaryLineEdit = nullptr;
    QTextEdit* m_descriptionTextEdit = nullptr;

    QPushButton* m_commitButton = nullptr;
    QPushButton* m_cancelButton = nullptr;

    QVBoxLayout* m_mainLayout = nullptr;

private:
    void initializeUi();
    void createWidgets();
    void createLayouts();
    void setupConnections();
    void updateRepositoryInfo();
    void updateUiState();
};

#endif // COMMITDIALOG_H
