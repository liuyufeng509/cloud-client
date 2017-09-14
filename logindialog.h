#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
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

private:
    void moveToCenter();
    Ui::LoginDialog *ui;
    QString serverIP;
    bool m_Drag;
    QPoint m_DragPosition;
};

#endif // LOGINDIALOG_H
