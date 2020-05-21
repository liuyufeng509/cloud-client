#include <QDesktopWidget>
#include<QMessageBox>
#include <qdebug.h>
#include <QEvent>
#include <QMouseEvent>
#include <QShortcut>
#include <QMetaType>
#include "logindialog.h"
#include "ui_logindialog.h"
#include <QRegExpValidator>


LoginDialog::LoginDialog(UserInfo &usrInfo, QWidget *parent) :
    QDialog(parent),
    m_userInfo(usrInfo),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    moveToCenter();

    QRegExp ipRegExp = QRegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExpValidator *ipRegExpValidator = new QRegExpValidator(ipRegExp, this);

    ui->serverLineEdit->setValidator(ipRegExpValidator);

    ui->serverLineEdit->setFocus();
    ui->loginPushButton->setShortcut(Qt::Key_Enter);

    QShortcut *key=new QShortcut(QKeySequence(Qt::Key_Enter), this);// 创建一个快捷键"Key_Return"键
    connect(key, SIGNAL(activated()), this, SLOT(on_loginPushButton_clicked()));//连接到指定槽函数

    //thread
    worker = new Worker;
    worker->moveToThread(&workerThread);
    qRegisterMetaType<UserInfo> ("UserInfo &");
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &LoginDialog::operate, worker, &Worker::doLogin);
    connect(worker, &Worker::loginReady, this, &LoginDialog::handleLoginRes);
    workerThread.start();
}

bool LoginDialog::Login()
{
    QString cmd = "/usr/bin/getLoginStatus.py "+m_userInfo.uname+" "+m_userInfo.pwd+" "+serverIP + " 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        qDebug()<<tr("Login failed: printed info nums less than 2");
        return false;
    }
    if(list.first()=="login success")
    {
        m_userInfo.uid = list.last();
        return true;
    }else
    {
        m_userInfo.uid = "";
        return false;
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}

/*鼠标按下事件*/
void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_Drag = true;
        m_DragPosition = event->globalPos() - this->pos();
        event->accept();
    }
}

/*鼠标移动事件*/
void LoginDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Drag && (event->buttons() && Qt::LeftButton)) {
        move(event->globalPos() - m_DragPosition);
        event->accept();
    }
}

/*鼠标释放事件*/
void LoginDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_Drag = false;
}

void LoginDialog::on_loginPushButton_clicked()
{
    int pos = 0;
    this->serverIP = ui->serverLineEdit->text();
    m_userInfo.uname = ui->usernameLineEdit->text();
    m_userInfo.pwd=ui->passwdLineEdit->text();

    if(this->serverIP.isEmpty())
    {
        ui->serverLineEdit->setFocus();
        ui->serverLineEdit->setCursorPosition(ui->serverLineEdit->text().length());
        ui->serverLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

    if(ui->serverLineEdit->validator()->validate(this->serverIP, pos) != QValidator::Acceptable)
    {
        ui->serverLineEdit->setFocus();
        ui->serverLineEdit->setCursorPosition(ui->serverLineEdit->text().length());
        ui->serverLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

    ui->serverLineEdit->setStyleSheet("background-color: white;");

    if (m_userInfo.uname.isEmpty())
    {
        ui->usernameLineEdit->setFocus();
        ui->usernameLineEdit->setCursorPosition(ui->usernameLineEdit->text().length());
        ui->usernameLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

    ui->usernameLineEdit->setStyleSheet("background-color: white;");

    if(m_userInfo.pwd.isEmpty())
    {
        ui->passwdLineEdit->setFocus();
        ui->passwdLineEdit->setCursorPosition(ui->passwdLineEdit->text().length());
        ui->passwdLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

    ui->passwdLineEdit->setStyleSheet("background-color: white;");
    worker->setSvrIP(serverIP);
    emit operate(m_userInfo);
    waitDiaogAppear();
//    if(Login())
//        accept();
//    else
//        QMessageBox::warning(this, "警告", "登录失败,请确认用户名和密码无误");
}

void LoginDialog::handleLoginRes(bool success)
{
    waitDialogAccept();
    if(success)
    {
        accept();
    }else
    {
        QMessageBox::warning(this, "警告", "登录失败,请确认用户名和密码无误");
    }
}

void LoginDialog::on_quitPushButton_clicked()
{
    reject();
}

void LoginDialog::moveToCenter()
{
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}

void LoginDialog::waitDialogAccept()
{
    waitD->accept();
    delete waitD;
}

void LoginDialog::waitDiaogAppear()
{
    waitD = new WaitDialog(this);
    waitD->exec();
  //  waitD->deleteLater();
}



