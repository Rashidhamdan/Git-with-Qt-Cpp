#ifndef INITREPODIALOG_H
#define INITREPODIALOG_H

#include <QDialog>

namespace Ui {
class InitRepoDialog;
}

class InitRepoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InitRepoDialog(QWidget *parent = nullptr);
    ~InitRepoDialog();

private:
    Ui::InitRepoDialog *ui;
};

#endif // INITREPODIALOG_H

