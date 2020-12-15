#ifndef SETAPPRELATIONDIALOG_H
#define SETAPPRELATIONDIALOG_H

#include <QDialog>

namespace Ui {
class SetAppRelationDialog;
}

class SetAppRelationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetAppRelationDialog(QWidget *parent = nullptr);
    ~SetAppRelationDialog();

private slots:
    void on_pushButton_clicked();

    void on_saveButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::SetAppRelationDialog *ui;
};

#endif // SETAPPRELATIONDIALOG_H
