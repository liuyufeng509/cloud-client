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
#include "scale/frameless_helper.h"
#include   "qflowlayout.h"
#include "movie/carouselimagewindow.h"
#include "qmessagehandles.h"
#include <QDesktopServices>
#include "rdesktoptip.h"
#include <QProcess>

HomeWindow::HomeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(minWidth, minHeight);    //最小值设为700*500

    m_flowLayout = new QFlowLayout;
    ui->vmsWidget->setLayout(m_flowLayout);

    moveToCenter();

    connect(QMessageHandles::instance(), &QMessageHandles::DeskTipMessage,this,[=](QStringList sl, int timeout){
        RDesktopTip::showTip(sl, timeout);
    });

    //thread
    worker = new Worker;
    worker->moveToThread(&workerThread);
    qRegisterMetaType<VMVECTOR> ("QVector<VM_CONFIG> *");
    qRegisterMetaType<QString> ("QString &");
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    //connect(this, &HomeWindow::getAllInfo, worker, &Worker::doGetAllInfo);
    connect(worker, &Worker::getAllInfoReady, this, &HomeWindow::handleGetAllInfoRes);
    workerThread.start();

    //get vms
    worker->setSvrIP(serverIp);
    worker->setUserInfo(m_userInfo);
//    emit getAllInfo(&vmArray,vms);
//    waitDiaogAppear();

    //initUI
    initTitleBar();
    setWindowIcon(QIcon(":/new/index/taskbar"));
    this->setWindowFlags(Qt::FramelessWindowHint);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);    // 设置尺寸属性
   // setMouseTracking(false);    // 界面拉伸需要这个属性

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


    //使用拖拽拉伸功能
    FramelessHelper *pHelper = new FramelessHelper(this->centralWidget());
    pHelper->activateOn(this);  //激活当前窗体
    pHelper->setTitleHeight(m_titleBar->height());  //设置窗体的标题栏高度
    //pHelper->setTitleHeight(30);
    pHelper->setWidgetMovable(true);  //设置窗体可移动
    pHelper->setWidgetResizable(true);  //设置窗体可缩放
    pHelper->setRubberBandOnMove(true);  //设置橡皮筋效果-可移动
    pHelper->setRubberBandOnResize(true);  //设置橡皮筋效果-可缩放

    m_picDown = new HttpPicDownLoad(this);

    connect(m_picDown, &HttpPicDownLoad::alreadyDownload, this, &HomeWindow::addImage);

    //幻灯片效果
//    ui->movieWidget->addImage("C:\\Users\\Administrator\\Desktop\\test\\1.png");
//    ui->movieWidget->addImage("C:\\Users\\Administrator\\Desktop\\test\\2.png");
//    ui->movieWidget->addImage("C:\\Users\\Administrator\\Desktop\\test\\3.png");
//    ui->movieWidget->startPlay();

//    m_picDown->downLoadPic("http://c.runoob.com/wp-content/uploads/2017/01/img_fjords_wide.jpg");
//    m_picDown->downLoadPic("http://c.runoob.com/wp-content/uploads/2017/01/img_nature_wide.jpg");
//    m_picDown->downLoadPic("http://c.runoob.com/wp-content/uploads/2017/01/img_mountains_wide.jpg");
//    m_picDown->addUrl("http://c.runoob.com/wp-content/uploads/2017/01/img_fjords_wide.jpg");
//    m_picDown->addUrl("http://c.runoob.com/wp-content/uploads/2017/01/img_nature_wide.jpg");
//    m_picDown->addUrl("http://c.runoob.com/wp-content/uploads/2017/01/img_mountains_wide.jpg");
    m_picDown->addUrl("http://localhost/images/1.jpg");
    m_picDown->addUrl("http://localhost/images/2.jpg");
    m_picDown->addUrl("http://localhost/images/3.jpg");
    m_picDown->downLoadPic();


    //服务列表
    connect(QMessageHandles::instance(), &QMessageHandles::servicesList, this, &HomeWindow::getAllServices);
    connect(QMessageHandles::instance(), &QMessageHandles::UpdateServciesList, this, &HomeWindow::UpdateServciesList);

    //初始化语言
    LoginDialog dia(m_userInfo);
    if (QDialog::Accepted == dia.exec())
    {
        this->serverIp = dia.getServerIP();
        qDebug() << this->serverIp << endl;
        ui->label->setText("<html><head/><body><p><span style=\" font-size:14pt; font-weight:600; color:#458b67;\">你好，"+m_userInfo.uname+"，欢迎中车VPN客户端！</span></p></body></html>");
    }
    else
    {
        exit(1);
    }
}

void HomeWindow::UpdateServciesList(QString services)
{
    //evtID:237, szPraram:{"rspID":237,"rspMsg":{"RawMsg":"-GroupListEnd- 5\t ftp_5\t 0\t 1\t 0\t 0\t 0\t 3\t \t ftp_5,192.168.70.5,192.168.70.5,192.168.70.5,192.168.70.5\n 1\t 统一运维平台\t 0\t 1\t 0\t 0\t 0\t 1\t \t 统一运维平台,10.20.0.231,10.20.0.231,10.20.0.231,10.20.0.231\n 2\t 重庆地铁智能运维系统\t 0\t 1\t 0\t 0\t 0\t 1\t \t 重庆地铁智能运维系统,192.168.71.10,192.168.71.10,192.168.71.10,192.168.71.10\n 6\t ftp_12\t 0\t 1\t 0\t 0\t 0\t 3\t \t ftp_12,192.168.70.12,192.168.70.12,192.168.70.12,192.168.70.12\n 3\t nginx代理-重庆\t 0\t 1\t 0\t 0\t 0\t 1\t \t nginx代理-重庆,192.168.70.5,192.168.70.5,192.168.70.5,192.168.70.5"}}
    QJsonParseError err_rpt;
    QJsonDocument  root_Doc = QJsonDocument::fromJson(services.toUtf8(), &err_rpt);//字符串格式化为JSON
    if(err_rpt.error != QJsonParseError::NoError)
    {
        qDebug()<<"服务详细信息json串解析失败："<<err_rpt.errorString();
        return;
    }else {

        QJsonObject root_Obj = root_Doc.object();
        QJsonObject rspMsg_Value = root_Obj.value("rspMsg").toObject();
        QString  rawMsg = rspMsg_Value.value("RawMsg").toString();
        rawMsg.replace("-GroupListEnd-","");
        QStringList serList = rawMsg.split("\n");
        for(int i=0;i<serList.count();i++)
        {
            QString serv = serList.at(i);
            QStringList infoList = serv.split("\t");
            if(infoList.size()!=10)
            {
                continue;
            }

            int id = infoList.at(0).trimmed().toInt();
            QString displayName = infoList.at(1).trimmed();
            int accessType = infoList.at(3).trimmed().toInt();
            int servType = infoList.at(7).trimmed().toInt();
            QString content = infoList.at(9).trimmed();
            QStringList sl = content.split(",");
            if(sl.size()<2)
            {
                continue;
            }
            QString ip =sl.at(1).trimmed();

            //找到对应的服务，更新信息
            for(int j=0;j<servicesList.size();j++)
            {
                if(servicesList[j].id==id && servicesList[j].displayName==displayName&&
                   servicesList[j].accessType == accessType && servicesList[j].servType==servType)
                {
                    servicesList[j].ip = ip;
                    break;
                }
            }

        }
    }

    updateViewUI();
}

void HomeWindow::getAllServices(QString serviceList)
{
    //evtID:208, szPraram:{"rspID":208,"rspMsg":{"ServiceCount":5,"ServiceList":[{"AccessType":1,"DisplayName":"ftp_5","ID":5,"ServType":3},{"AccessType":1,"DisplayName":"统一运维平台","ID":1,"ServType":1},{"AccessType":1,"DisplayName":"重庆地铁智能运维系统","ID":2,"ServType":1},{"AccessType":1,"DisplayName":"ftp_12","ID":6,"ServType":3},{"AccessType":1,"DisplayName":"nginx代理-重庆","ID":3,"ServType":1}]}}

    QJsonParseError err_rpt;
    QJsonDocument  root_Doc = QJsonDocument::fromJson(serviceList.toUtf8(), &err_rpt);//字符串格式化为JSON
    if(err_rpt.error != QJsonParseError::NoError)
    {
        qDebug()<<"服务列表json串解析失败："<<err_rpt.errorString();
        return;
    }else {

        QJsonObject root_Obj = root_Doc.object();
        QJsonObject rspMsg_Value = root_Obj.value("rspMsg").toObject();
        int serviceCount = rspMsg_Value.value("ServiceCount").toInt();
        QJsonArray servicelist_value = rspMsg_Value.value("ServiceList").toArray();
        if(servicelist_value.size()!=serviceCount)
        {
            qDebug()<<"解析的servicecount与数组大小不一致";
            return;
        }
        servicesList.clear();
        for(int i=0; i<serviceCount;i++)
        {
            QJsonObject server_obj = servicelist_value.at(i).toObject();
            Service serv;
            serv.accessType = server_obj.value("AccessType").toInt();
            serv.displayName = server_obj.value("DisplayName").toString();
            serv.id = server_obj.value("ID").toInt();
            serv.servType = server_obj.value("ServType").toInt();
            servicesList.append(serv);
        }

    }

    updateViewUI();
}

void HomeWindow::addImage(QString path, int counter)
{
    ui->movieWidget->addImage(path);
    if(counter==3)
        ui->movieWidget->startPlay();

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
//    if(!success)
//    {
//        QMessageBox::warning(this, "警告", "获取虚拟机信息失败，请尝试刷新");
//    }

    VM_CONFIG vm1;
    vm1.name = "vm1";
    vm1.vid="asdfbgasdgkjldajdf";
    vm1.ip="192.168.0.1";
    vm1.port = 22;
    vm1.status = RUNING;

    VM_CONFIG vm2;
    vm2.name = "vm2";
    vm2.vid="asdfbgasdgkjldajdf";
    vm2.ip="192.168.0.2";
    vm2.port = 22;
    vm2.status = SHUTDOWN;

    vmArray.push_back(vm1);
    vmArray.push_back(vm2);

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

void HomeWindow::resizeEvent(QResizeEvent *e)
{
    //qDebug()<<"resize Event happend";
   // updateViewUI();
    QMainWindow::resizeEvent(e);
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
    //clearLayout(ui->vmsGridLayout);
    clearLayout(m_flowLayout);
    //计算每行放几个vm
    for(int i=0; i<servicesList.size(); i++)
   {
       VMWidget *vm = new VMWidget(servicesList[i],this);
       int k = this->width()/vm->width();
       //ui->vmsGridLayout->addWidget(vm, i/k, i%k);
        m_flowLayout->addWidget(vm);
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

    //openVm(vmArray[row]);
}

void HomeWindow::openVm(Service vm)
{
   // qDebug()<<"服务的ip:"<<vm.displayName<<" "<<vm.ip;
    vm.getUrlandIcon();
    if(vm.appUrl.isEmpty())
        QDesktopServices::openUrl(QUrl(vm.url));
    else{
        QString path=vm.appUrl;
        if(vm.arg=="%S")
        {
            path = "\""+path+"\""+ " "+vm.url;
        }else {
           path = "\""+path+"\""+" "+vm.url +" "+vm.arg;
        }
        QProcess process;
        process.startDetached(path);
    }

}

void HomeWindow::initTitleBar()
{
    m_titleBar = new MyTitleBar(this);
    m_titleBar->move(0, 0);
    //m_titleBar->setTitleIcon(":/new/index/mainwintitle",QSize(145, 30));
    m_titleBar->setTitleIcon(":new/vpn/vpnlogo",QSize(145, 30));
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
    LogoutStatus stat = funcLogoutSslVpn();
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
