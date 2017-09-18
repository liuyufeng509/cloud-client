#include "homewindow.h"
#include "ui_homewindow.h"
#include <QSettings>
#include <QFile>
#include <QApplication>
#include <QTranslator>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>
#include "simple_log.h"
#include "vmwidget.h"
#include <QtScript/QtScript>
#include <QMenu>
#include <QEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include "logindialog.h"
#include "3rd/json/json/json.h"
#include <qdebug.h>
#include <QTcpSocket>
#include <QHostAddress>

#include <QJsonParseError>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonDocument>

HomeWindow::HomeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    moveToCenter();

    //初始化语言

    LoginDialog dia(m_userInfo);
    if (QDialog::Accepted == dia.exec())
    {
        this->serverIp = dia.getServerIP();
        qDebug() << this->serverIp << endl;
    }
    else
    {
        exit(1);
    }

    //get vms
    getVMs();
    getVMsIpPort();
    getVMsInfo();
//    for(int i=0;i<vmArray.size();i++)
//    {
//        qDebug()<<"***************************************";
//        qDebug()<<"vmArray["<<i<<"].name="<<vmArray[i].name;
//        qDebug()<<"vmArray["<<i<<"].vid="<<vmArray[i].vid;
//        qDebug()<<"vmArray["<<i<<"].ip="<<vmArray[i].ip;
//        qDebug()<<"vmArray["<<i<<"].port="<<vmArray[i].port;
//        qDebug()<<"vmArray["<<i<<"].status="<<vmArray[i].status;
//        qDebug()<<"vmArray["<<i<<"].created="<<vmArray[i].created;
//        for(int j=0;j<vmArray[i].addrs.size();j++)
//        {
//            qDebug()<<"vmArray["<<i<<"].addr["<<j<<"].ip="<<vmArray[i].addrs[j].ip;
//            qDebug()<<"vmArray["<<i<<"].addr["<<j<<"].mac="<<vmArray[i].addrs[j].mac;
//            qDebug()<<"vmArray["<<i<<"].addr["<<j<<"].type="<<vmArray[i].addrs[j].type;
//        }
//    }

    //initUI
    initTitleBar();
    setWindowIcon(QIcon(":/new/index/taskbar"));
    this->setWindowFlags(Qt::FramelessWindowHint);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);    // 设置尺寸属性
    setMouseTracking(true);    // 界面拉伸需要这个属性

    updateUI();
}

void HomeWindow::updateUI()
{
    for(int i=0; i<vmArray.size(); i++)
   {
       VMWidget *vm = new VMWidget(vmArray[i],this);

       ui->vmsGridLayout->addWidget(vm, i/4, i%4);
       connect(vm, &VMWidget::emitData, this, &HomeWindow::openVm);
   }
}

void HomeWindow::openVm(VM_CONFIG vm)
{
    if(vm.status!=RUNING)
    {
        QMessageBox::information(this, "提示","该虚拟主机不在线");
        return;
    }
    QString cmd = QString("%1 spice://%2:%3 --full-screen  &").arg("/usr/bin/remote-viewer").arg(vm.ip).arg(vm.port);
    GetCmdRes(cmd);
}

void HomeWindow::initTitleBar()
{
    m_titleBar = new MyTitleBar(this);
    m_titleBar->move(0, 0);
    m_titleBar->setTitleIcon(":/new/index/mainwintitle",QSize(145, 30));
    m_titleBar->setTitleContent(tr("方德云客户端"), 11);
    m_titleBar->setBackgroundColor(69, 139, 103);
    m_titleBar->setButtonType(MIN_MAX_BUTTON);
    connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
    connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));

}

void HomeWindow::onButtonMinClicked()
{
    showMinimized();
}

void HomeWindow::onButtonRestoreClicked()
{
    QPoint windowPos;
    QSize windowSize;
    m_titleBar->getRestoreInfo(windowPos, windowSize);
    this->setGeometry(QRect(windowPos, windowSize));
}

void HomeWindow::onButtonMaxClicked()
{
    m_titleBar->saveRestoreInfo(this->pos(), QSize(this->width(), this->height()));
    QRect desktopRect = QApplication::desktop()->availableGeometry();
    QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
    setGeometry(FactRect);
}

void HomeWindow::onButtonCloseClicked()
{
    close();
}


bool HomeWindow::getVMsIpPort()
{
    QString cmd = "telnet "+serverIp + " "+QString::number(59000) +" 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<5)
    {
        qDebug()<<tr("telnet: info nums less than 5");
        return false;
    }
    for(int i=0; i<list.size();i++)
    {
        if(list[i].contains("\"instances\"")&&parseVMsIpPort(list[i]))
            return true;
    }
    return false;
}

bool HomeWindow::parseVMsIpPort(QString output)
{
    QScriptEngine engine;
    QScriptValue sc = engine.evaluate("value=" + output);

    if( sc.isArray())
    {
        QScriptValueIterator it(sc);
        while(it.hasNext())
        {
            it.next();

            QScriptValueIterator i(it.value().property("instances"));
            while(i.hasNext())
            {
                i.next();
                QString uuid = i.value().property("uuid").toString();
                for(int j=0; j<vmArray.size();j++)
                {
                    if(vmArray[j].vid == uuid)
                    {
                        vmArray[j].ip = i.value().property("hostip").toString();
                        vmArray[j].port = i.value().property("spiceport").toInt32();
                        if(vmArray[j].port == -1)
                        {
                            vmArray[j].status = SHUTDOWN;
                        }else
                            vmArray[j].status = RUNING;
                        break;
                    }
                }

            }
        }
    }
}

bool HomeWindow::parseVMsInfo(QByteArray &ba)
{
    QJsonParseError jsonpe;
    QJsonDocument json = QJsonDocument::fromJson(ba, &jsonpe);
    if(jsonpe.error == QJsonParseError::NoError)
    {
        if(json.isArray())
        {
            QJsonArray vmsobjs = json.array();
            for(int i=0; i<vmsobjs.size(); i++)
            {
                QJsonObject obj = vmsobjs[i].toObject();
                QString id = obj["id"].toString();
                for(int j=0; j<vmArray.size();j++)
                {
                    if(id == vmArray[j].vid)
                    {
                        QJsonObject obj1 = obj["addresses"].toObject();
                        QJsonArray array = obj1["demo"].toArray();
                        for(int k=0; k<array.size();k++)
                        {
                            Addr addr;
                            addr.mac = (array[k].toObject())["OS-EXT-IPS-MAC:mac_addr"].toString();
                            addr.ip = (array[k].toObject())["addr"].toString();
                            addr.type = (array[k].toObject())["OS-EXT-IPS:type"].toString();
                            vmArray[j].addrs.append(addr);
                        }
                        vmArray[j].created =obj["created"].toString();
                        break;
                    }
                }

            }
        }
        return true;
    }else
        return false;
}

bool HomeWindow::getVMsInfo()
{
    QString cmd = "/root/getVMsInfo.py "+m_userInfo.uname+" "+m_userInfo.pwd+" "+serverIp + " " +vms+" 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        qDebug()<<tr("Login failed: printed info nums less than 2");
        return false;
    }
    if(list.contains("login failed"))
    {
        QMessageBox::information(this, "警告", "获取虚拟机详细信息失败，请点击刷新，重新获取");
        return false;
    }else if(list.contains("login success"))
    {
        QByteArray cstr = list.last().toLatin1();
        if(!parseVMsInfo(cstr))
            QMessageBox::information(this, "警告", "解析虚拟机详细信息失败，请点击刷新，重新获取");
        return true;
    }
    return true;

}

bool HomeWindow::getVMs()
{
    QString cmd = "/root/getVMs.py "+m_userInfo.uname+" "+m_userInfo.pwd+" "+serverIp + " 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        qDebug()<<tr("Login failed: printed info nums less than 2");
        return false;
    }
    if(list.contains("login failed"))
    {
        QMessageBox::information(this, "警告", "获取虚拟机列表信息失败，请点击刷新，重新获取");
        return false;
    }else if(list.contains("login success"))
    {
        QByteArray cstr = list.last().toLatin1();
        if(!parseVMs(cstr))
            QMessageBox::information(this, "警告", "解析虚拟机列表信息失败，请点击刷新，重新获取");
        vms = '\''+list.last()+'\'';

        return true;
    }
    return true;
}

bool HomeWindow::parseVMs(QByteArray &ba)
{
    vmArray.clear();
    QJsonParseError jsonpe;
    QJsonDocument json = QJsonDocument::fromJson(ba, &jsonpe);

    if(jsonpe.error == QJsonParseError::NoError)
    {
        if(json.isArray())
        {
            QJsonArray vmsobjs = json.array();
            for(int i=0; i<vmsobjs.size(); i++)
            {
                QJsonObject obj = vmsobjs[i].toObject();
                VM_CONFIG vm;
                vm.name = obj["name"].toString();
                vm.vid = obj["uuid"].toString();
                vmArray.append(vm);
            }
        }
        return true;
    }else
        return false;
}

void HomeWindow::moveToCenter()
{
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}



HomeWindow::~HomeWindow()
{
    delete ui;
}



//添加快捷键
void HomeWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() == Qt::ControlModifier)
    {
            if(e->key() == Qt::Key_F10)
            {
                setWindowState(Qt::WindowFullScreen);
            }
            if(e->key() == Qt::Key_F11)
            {
                setWindowState(Qt::WindowMinimized);
            }

            if(e->key() == Qt::Key_F12)
            {
                setWindowState(Qt::WindowNoState);
            }
    }
    if(e ->modifiers() == Qt::Key_Meta)
    {
        return;
    }
    if(e ->modifiers() == Qt::AltModifier  && e ->key() == Qt::Key_Tab)
    {
        e->ignore();
    }
    else
    {
        QWidget::keyPressEvent(e);
    }
}



/*启动VDI客户端*/
//void HomeWindow::button_pressed_slot(int index)
//{
//    QString lang;

//    if (m_curLang == "english")
//    {
//        lang = "en_US.UTF-8";
//    }
//    else
//    {
//        lang = "zh_CN.UTF-8";
//    }

//    VM_CONFIG vm = vmArray[index];

//    QStringList globalArgs = qApp->arguments();
//    qDebug() << globalArgs;

//    QString cmd = QString("%1 spice://%2:%3 --full-screen").arg(/*globalArgs[1]*/"/usr/bin/remote-viewer").arg(vm.ip).arg(vm.port);

//    LOG(DEBUG_LEVEL, cmd.toStdString().c_str());

//    system(cmd.toStdString().c_str());

//    return;
//}

