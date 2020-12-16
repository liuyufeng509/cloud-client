#ifndef SETAPPRELATIONDIALOG_H
#define SETAPPRELATIONDIALOG_H

#include <QDialog>
#include"common.h"
class QSettings;
namespace Ui {
class SetAppRelationDialog;
}

class SetAppRelationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetAppRelationDialog(Service & serv,QWidget *parent = nullptr);
    ~SetAppRelationDialog();

private slots:
    void on_pushButton_clicked();

    void on_saveButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::SetAppRelationDialog *ui;
    QString appName;
    Service &m_serv;
    QList<Service> m_servs;
};

#endif // SETAPPRELATIONDIALOG_H
