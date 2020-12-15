#include "setapprelationdialog.h"
#include "ui_setapprelationdialog.h"

SetAppRelationDialog::SetAppRelationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetAppRelationDialog)
{
    ui->setupUi(this);
}

SetAppRelationDialog::~SetAppRelationDialog()
{
    delete ui;
}

void SetAppRelationDialog::on_pushButton_clicked()
{

}

void SetAppRelationDialog::on_saveButton_clicked()
{
    accept();
}

void SetAppRelationDialog::on_cancelButton_clicked()
{
    reject();
}
