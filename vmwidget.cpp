#include "vmwidget.h"
#include "ui_vmwidget.h"
#include "detaildialog.h"
#include "homewindow.h"
#include<unistd.h>
#include<QIcon>
#include<QFile>
#include "setapprelationdialog.h"
#include<QStandardPaths>
VMWidget::VMWidget(Service &vm,QWidget *parent) :
    QWidget(parent),
    m_vm(vm),
    prt(parent),
    ui(new Ui::VMWidget)
{
    ui->setupUi(this);

    setStyleSheetByStatus();
    ui->label_vmName->setText(m_vm.displayName);
    ui->VMButton->setContextMenuPolicy(Qt::CustomContextMenu);
    m_menu = new QMenu(this);
    setAction = new QAction( QIcon(":/new/vpn/set"),"设置启动程序",this);
    exportAction = new QAction(QIcon(":/new/vpn/export"),"创建快捷方式",this);

    connect(setAction, SIGNAL(triggered()), this, SLOT(operateActionSlot()));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(detailActionSlot()));

    m_menu->addAction(setAction);
    m_menu->addAction(exportAction);

    connect(ui->VMButton, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu(const QPoint&)));

    worker = new Worker;

}

void VMWidget::operateActionSlot()
{
    //startup or shutdown
    SetAppRelationDialog dlg(m_vm,this);
    dlg.exec();

}

void VMWidget::detailActionSlot()
{
    //快捷方式
    /* *.url
     * [InternetShortcut]
URL=http://Kennytian.cnblogs.com
     * */
    QString location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString filename = location+"/"+m_vm.displayName+".url";
    QFile file(filename);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream ts(&file);
        ts<<"[InternetShortcut]"<<"\n";
        ts<<"URL="+m_vm.url<<"\n";
        ts<<"IconIndex=0"<<"\n";
        ts<<"HotKey=0"<<"\n";
        ts<<"IDList="<<"\n";
        ts<<"IconFile="+qApp->applicationFilePath()<<"\n";
        ts<<"[{000214A0-0000-0000-C000-000000000046}]"<<"\n";
        ts<<"Prop3=19,1"<<"\n";
        file.close();
    }
}

void VMWidget::showMenu(const QPoint &point)
{
    m_menu->exec(mapToGlobal(point));
}

void VMWidget::setStyleSheetByStatus()
{
    m_vm.getUrlandIcon();
    setStyleSheet(" QPushButton {  \
                      border: none; \
                      border-radius: 0px;   \
                      color:white;  \
                      font-weight:bold; \
                      image: url("+m_vm.imageUrl+");   \
                  } \
                  QPushButton:hover { \
                      border: 1px solid white; \
                      border-radius: 0px; \
                      color:white; \
                      font-weight:normal; \
                      image: url("+m_vm.imageUrl+"); \
                  } \
                  QPushButton:pressed { \
                      border: 1px solid white; \
                      border-radius: 0px; \
                      color:white; \
                        opacity:0.4; \
                      font-weight:bold; \
                      image: url(/*"+m_vm.imageUrl+"*/); \
                  }");

}

VMWidget::~VMWidget()
{
    delete ui;
}

void VMWidget::on_VMButton_clicked()
{
    QString path = QApplication::applicationDirPath();
    path = path + QString("/")+ QString("servs");
    QFile file(path);
    if(file.open(QFile::ReadOnly))
    {
        QString res = file.readAll();
        QStringList sl = res.split('\n');
        for(int i=0;i<sl.size();i++)
        {
            QStringList servs = sl[i].split('#');
            if(servs.size()!=6)
                continue;
            Service s;
            s.accessType = servs[0].toInt();
            s.displayName=servs[1];
            s.id = servs[2].toInt();
            s.servType = servs[3].toInt();
            s.appUrl=servs[4];
            s.arg = servs[5];

            if(s==m_vm)
            {
                m_vm.appUrl = s.appUrl;
                m_vm.arg = s.arg;
            }
        }
        file.close();
    }
    emit emitData(m_vm);
}
