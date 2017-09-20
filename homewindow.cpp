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
#include<unistd.h>

#include <QJsonParseError>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonDocument>
#include "detaildialog.h"

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

    //initUI
    initTitleBar();
    setWindowIcon(QIcon(":/new/index/taskbar"));
    this->setWindowFlags(Qt::FramelessWindowHint);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);    // 设置尺寸属性
    setMouseTracking(true);    // 界面拉伸需要这个属性

    connect(ui->tabWidget, SIGNAL(currentChanged(int )), this, SLOT(tabChanged(int )));

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    QHeaderView* headerView = ui->tableWidget->verticalHeader();
    headerView->setHidden(true);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    ui->tableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &HomeWindow::openVmOfTable);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);   //右键菜单
    m_menu = new QMenu(ui->tableWidget);
    operAction = new QAction(this);
    detailAction = new QAction(this);
    detailAction->setText(tr("详细信息"));
    operAction->setText(tr("关机"));


    connect(operAction, SIGNAL(triggered()), this, SLOT(operateActionSlot()));
    connect(detailAction, SIGNAL(triggered()), this, SLOT(detailActionSlot()));

}

void HomeWindow::operateActionSlot()
{
    //startup or shutdown
    int row = ui->tableWidget->currentRow();
    if(row<0)
    {
        QMessageBox::information(NULL, "提示","请选择操作的虚拟机");
        return;
    }


    if(worker->operateVMs(vmArray[row].vid,&vmArray[row].status))
    {
        sleep(3);
        worker->getVMsIpPort(vmArray);
        updatetableUI();
        updateViewUI();

        QMessageBox::information(NULL, "提示","开关机操作成功");
    }
}

void HomeWindow::detailActionSlot()
{
    //detail info
    int row = ui->tableWidget->currentRow();
    if(row<0)
    {
        QMessageBox::information(NULL, "提示","请选择操作的虚拟机");
        return;
    }
    DetailDialog dlg(vmArray[row]);
    dlg.exec();
}



void HomeWindow::on_tableWidget_customContextMenuRequested(QPoint pos)
{
    m_menu->addAction(operAction);
    m_menu->addAction(detailAction);
    int row = ui->tableWidget->currentRow();
    if(row<0)
    {
        return;
    }
    if(vmArray[row].status == RUNING)
    {
        operAction->setText(tr("关机"));
    }else
        operAction->setText(tr("开机"));
    m_menu->exec(QCursor::pos());
    pos.isNull();
}

void HomeWindow::tabChanged(int index)
{
    if(index ==1)
        updatetableUI();
}

void HomeWindow::handleGetAllInfoRes(bool success)
{
    waitDialogAccept();
    if(!success)
    {
        QMessageBox::warning(this, "警告", "获取虚拟机信息失败，请尝试刷新");
    }
    updateViewUI();
    updatetableUI();
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

void HomeWindow::updatetableUI()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(vmArray.size());
    ui->tableWidget->setToolTip(tr("右键可对用户进行操作"));
    for(int i=0; i<vmArray.size(); i++)
    {
        QIcon icon;
        icon.addFile(":/new/index/run_namal", QSize(32,32));
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(icon,vmArray[i].name));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(vmArray[i].vid));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(vmArray[i].status)));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(vmArray[i].ip));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(vmArray[i].port)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(vmArray[i].created));
    }
}

void HomeWindow::updateViewUI()
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

void HomeWindow::openVmOfTable(int row, int column)
{
    if(row<0 || column<0)
    {
        QMessageBox::information(NULL, tr("提示"),tr("请选中要打开的虚拟机"));
        return;
    }

    openVm(vmArray[row]);
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

void HomeWindow::on_updateButton_clicked()
{
    if(worker->needUpdate())
    {
        if(QMessageBox::question(NULL, tr("提示"), tr("检测到新版本，是否需要更新？"))==QMessageBox::No)
        {
            return;
        }else
        {
            QString err = "";
            if(worker->update(err))
                QMessageBox::information(NULL, tr("提示"), tr("升级结束"));
            else
                QMessageBox::information(NULL, tr("提示"), tr("升级失败:")+err);
        }

    }else
    {
        QMessageBox::information(NULL, tr("提示"), tr("已是最新版本，不需要升级"));
    }

}
