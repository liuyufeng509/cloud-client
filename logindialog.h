#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include"common.h"
#include <QDialog>
#include "waitdialog.h"
#include <QThread>
#include "worker.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(UserInfo &usrInfo, QWidget *parent = 0);
    ~LoginDialog();
    QString getServerIP(){
        return serverIP;
    }


protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void on_loginPushButton_clicked();
    void on_quitPushButton_clicked();

    void handleLoginRes(bool success,QString errInfo);

    void loginTimeOut();

signals:
    void operate(UserInfo &userinfo);

private:
    void moveToCenter();
    void waitDiaogAppear();
    void waitDialogAccept();

    Ui::LoginDialog *ui;
    QString serverIP;
    bool m_Drag;
    QPoint m_DragPosition;
    // user info
    UserInfo  &m_userInfo;

    WaitDialog *waitD=nullptr;
    QThread workerThread;
    Worker *worker;
};

#endif // LOGINDIALOG_H
