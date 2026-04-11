#include "InitRepoDialog.h"
#include "ui_InitRepoDialog.h"

InitRepoDialog::InitRepoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InitRepoDialog)
{
    ui->setupUi(this);
}

InitRepoDialog::~InitRepoDialog()
{
    delete ui;
}

