#include "vmwidget.h"
#include "ui_vmwidget.h"
#include "detaildialog.h"
#include "homewindow.h"
#include<unistd.h>
#include<QIcon>
#include "setapprelationdialog.h"
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
    SetAppRelationDialog dlg(this);
    dlg.exec();

}

void VMWidget::detailActionSlot()
{
    //快捷方式
    /* *.url
     * [InternetShortcut]
URL=http://Kennytian.cnblogs.com
     * */
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
    emit emitData(m_vm);
}
