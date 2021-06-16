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
#include "qmessagehandles.h"
#include <QTimer>
#include <QSettings>

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
    connect(this, &LoginDialog::operate, worker, &Worker::Login);
    connect(QMessageHandles::instance(), &QMessageHandles::loginReady, this, &LoginDialog::handleLoginRes);
    workerThread.start();
    readLoginInfo();
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
    m_userInfo.ip = ui->serverLineEdit->text();
    m_userInfo.port = ui->portLineEdit->text();
    m_userInfo.uname = ui->usernameLineEdit->text();
    m_userInfo.pwd=ui->passwdLineEdit->text();
    m_userInfo.otp = ui->otpLineEdit->text();


    //ip合法性判断
    if(m_userInfo.ip.isEmpty())
    {
        ui->serverLineEdit->setFocus();
        ui->serverLineEdit->setCursorPosition(ui->serverLineEdit->text().length());
        ui->serverLineEdit->setStyleSheet("background-color: yellow;color:black;");
        return;
    }

    if(ui->serverLineEdit->validator()->validate(m_userInfo.ip, pos) != QValidator::Acceptable)
    {
        ui->serverLineEdit->setFocus();
        ui->serverLineEdit->setCursorPosition(ui->serverLineEdit->text().length());
        ui->serverLineEdit->setStyleSheet("background-color: yellow;color:black;");
        return;
    }

    ui->serverLineEdit->setStyleSheet("background-color: white;color:black;");

    //端口号合法性判断
    if (m_userInfo.port.isEmpty())
    {
        ui->portLineEdit->setFocus();
        ui->portLineEdit->setCursorPosition(ui->portLineEdit->text().length());
        ui->portLineEdit->setStyleSheet("background-color: yellow;color:black;");
        return;
    }

    ui->portLineEdit->setStyleSheet("background-color: white;color:black;");

    //用户名合法性判断
    if (m_userInfo.uname.isEmpty())
    {
        ui->usernameLineEdit->setFocus();
        ui->usernameLineEdit->setCursorPosition(ui->usernameLineEdit->text().length());
        ui->usernameLineEdit->setStyleSheet("background-color: yellow;color:black;");
        return;
    }

    ui->usernameLineEdit->setStyleSheet("background-color: white;color:black;");

    //密码合法性判断
    if(m_userInfo.pwd.isEmpty())
    {
        ui->passwdLineEdit->setFocus();
        ui->passwdLineEdit->setCursorPosition(ui->passwdLineEdit->text().length());
        ui->passwdLineEdit->setStyleSheet("background-color: yellow;color:black;");
        return;
    }
    ui->passwdLineEdit->setStyleSheet("background-color: white;color:black;");

    //动态口令合法性判断
    if(m_userInfo.otp.isEmpty())
    {
        ui->otpLineEdit->setFocus();
        ui->otpLineEdit->setCursorPosition(ui->otpLineEdit->text().length());
        ui->otpLineEdit->setStyleSheet("background-color: yellow;color:black;");
        return;
    }
    ui->passwdLineEdit->setStyleSheet("background-color: white;color:black;");
    //记住登录信息
    saveLoginInfo();
    //worker->setSvrIP(serverIP);
    QTimer::singleShot(5000, this, SLOT(loginTimeOut()));
    emit operate(m_userInfo);
    waitDiaogAppear();

}

void LoginDialog::saveLoginInfo()
{
    QString path = QApplication::applicationDirPath();
    path = path + QString("/")+ QString("login.ini");
    QSettings settings(path, QSettings::IniFormat);
    settings.setValue("login/ip", ui->serverLineEdit->text());
    settings.setValue("login/port", ui->portLineEdit->text());
    settings.setValue("login/username", ui->usernameLineEdit->text());
}

void LoginDialog::readLoginInfo()
{
    QString path = QApplication::applicationDirPath();
    path = path + QString("/")+ QString("login.ini");
    QSettings settings(path, QSettings::IniFormat);
    ui->serverLineEdit->setText(settings.value("login/ip", "").toString());
    ui->portLineEdit->setText(settings.value("login/port", "").toString());
    ui->usernameLineEdit->setText(settings.value("login/username", "").toString());
}


void LoginDialog::loginTimeOut()
{
    waitDialogAccept();
    switch (worker->stat) {
        case ERROR_LOGIN_FAILED:
            QMessageBox::warning(this, "警告", "密码或口令错误，请重新填写登录信息");
            return;
    }
    QMessageBox::warning(this, "警告", "登录超时，请检查网络连接");
}

void LoginDialog::handleLoginRes(bool success,QString errInfo)
{
    waitDialogAccept();
    if(success)
    {
        accept();
    }else
    {
        QMessageBox::warning(this, "警告", errInfo);
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
    if(waitD!=nullptr)
    {
        waitD->accept();
        waitD->deleteLater();
        waitD = nullptr;
    }
}

void LoginDialog::waitDiaogAppear()
{
    waitD = new WaitDialog(this);
    waitD->exec();
  //  waitD->deleteLater();
}



