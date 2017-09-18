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
        ui->label->setText("<html><head/><body><p><span style=\" font-size:14pt; font-weight:600; color:#458b67;\">你好，"+m_userInfo.uname+"，欢迎使用方德云客户端！</span></p></body></html>");
    }
    else
    {
        exit(1);
    }

    //thread
    worker = new Worker;
    worker->moveToThread(&workerThread);
    qRegisterMetaType<VMVECTOR> ("QVector<VM_CONFIG> *");
    qRegisterMetaType<QString> ("QString &");
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &HomeWindow::getAllInfo, worker, &Worker::doGetAllInfo);
    connect(worker, &Worker::getAllInfoReady, this, &HomeWindow::handleGetAllInfoRes);
    workerThread.start();

    //get vms
    worker->setSvrIP(serverIp);
    worker->setUserInfo(m_userInfo);
    emit getAllInfo(&vmArray,vms);
    waitDiaogAppear();
//    getVMs();
//    getVMsIpPort();
//    getVMsInfo();
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

   // updateUI();
}

void HomeWindow::handleGetAllInfoRes(bool success)
{
    waitDialogAccept();
    if(!success)
    {
        QMessageBox::warning(this, "警告", "获取虚拟机信息失败，请尝试刷新");
    }
    updateUI();
}

void HomeWindow::clearLayout(QLayout *layout)
{
    QLayoutItem *item;
    while((item = layout->takeAt(0)) != 0){
        //删除widget
        if(item->widget()){
            delete item->widget();
            //item->widget()->deleteLater();
        }
        //删除子布局
        QLayout *childLayout = item->layout();
        if(childLayout){
            clearLayout(childLayout);
        }
        delete item;
    }
}

void HomeWindow::updateUI()
{
    clearLayout(ui->vmsGridLayout);
    for(int i=0; i<vmArray.size(); i++)
   {
       VMWidget *vm = new VMWidget(vmArray[i],this);
        vm->setSvrIP(serverIp);
        vm->setUserInfo(m_userInfo);
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

void HomeWindow::moveToCenter()
{
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}



HomeWindow::~HomeWindow()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
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

void HomeWindow::waitDialogAccept()
{
    waitD->accept();
    delete waitD;
}

void HomeWindow::waitDiaogAppear()
{
    waitD = new WaitDialog(this);
    waitD->exec();
  //  waitD->deleteLater();
}




void HomeWindow::on_freshButton_clicked()
{
    emit getAllInfo(&vmArray,vms);
    waitDiaogAppear();
//    clearLayout(ui->vmsGridLayout);
}

void HomeWindow::on_logoutButton_clicked()
{
    exit(0);
}
