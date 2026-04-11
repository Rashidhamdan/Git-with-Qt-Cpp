#include "StashDialog.h"
#include "ui_StashDialog.h"

StashDialog::StashDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StashDialog)
{
    ui->setupUi(this);
}

StashDialog::~StashDialog()
{
    delete ui;
}

