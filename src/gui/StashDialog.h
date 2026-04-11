#ifndef STASHDIALOG_H
#define STASHDIALOG_H

#include <QDialog>

namespace Ui {
class StashDialog;
}

class StashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StashDialog(QWidget *parent = nullptr);
    ~StashDialog();

private:
    Ui::StashDialog *ui;
};

#endif // STASHDIALOG_H

