#include <QDesktopWidget>
#include <qdebug.h>
#include <QEvent>
#include <QMouseEvent>
#include <QShortcut>

#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
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
}

LoginDialog::~LoginDialog()
{
    delete ui;
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
    QString username = ui->usernameLineEdit->text();
    QString password=ui->passwdLineEdit->text();

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

    if (username.isEmpty())
    {
        ui->usernameLineEdit->setFocus();
        ui->usernameLineEdit->setCursorPosition(ui->usernameLineEdit->text().length());
        ui->usernameLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

    ui->usernameLineEdit->setStyleSheet("background-color: white;");

    if(password.isEmpty())
    {
        ui->passwdLineEdit->setFocus();
        ui->passwdLineEdit->setCursorPosition(ui->passwdLineEdit->text().length());
        ui->passwdLineEdit->setStyleSheet("background-color: yellow;");
        return;
    }

    ui->passwdLineEdit->setStyleSheet("background-color: white;");

    if(username != "admin" || password != "123456")
    {
        ui->usernameLineEdit->setFocus();
        ui->usernameLineEdit->setCursorPosition(ui->usernameLineEdit->text().length());
        ui->passwdLineEdit->clear();
        ui->error_label->setText("用户名或密码错误!");
        return;
    }
    else
    {
        accept();
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
