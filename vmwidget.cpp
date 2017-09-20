#include "vmwidget.h"
#include "ui_vmwidget.h"
#include "detaildialog.h"
#include "homewindow.h"
#include<unistd.h>
VMWidget::VMWidget(VM_CONFIG &vm,QWidget *parent) :
    QWidget(parent),
    m_vm(vm),
    prt(parent),
    ui(new Ui::VMWidget)
{
    ui->setupUi(this);

    setStyleSheetByStatus();
    ui->label_vmName->setText(m_vm.name);
    ui->VMButton->setContextMenuPolicy(Qt::CustomContextMenu);
    m_menu = new QMenu(this);
    operAction = new QAction(this);
    detailAction = new QAction(this);
    detailAction->setText(tr("详细信息"));
    if(m_vm.status == RUNING)
    {
        operAction->setText(tr("关机"));
    }else
        operAction->setText(tr("开机"));

    connect(operAction, SIGNAL(triggered()), this, SLOT(operateActionSlot()));
    connect(detailAction, SIGNAL(triggered()), this, SLOT(detailActionSlot()));

    m_menu->addAction(operAction);
    m_menu->addAction(detailAction);

    connect(ui->VMButton, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu(const QPoint&)));

    worker = new Worker;

}

void VMWidget::operateActionSlot()
{
    //startup or shutdown
    if(worker->operateVMs(m_vm.vid,&m_vm.status))
        QMessageBox::information(NULL, "提示","开关机操作成功");
    setStyleSheetByStatus();
    HomeWindow *hw = static_cast<HomeWindow *>(prt);
    sleep(2);
    worker->getVMsIpPort(hw->vmArray);
    hw->updatetableUI();
}

void VMWidget::detailActionSlot()
{
    //detail info
    DetailDialog dlg(m_vm);
    dlg.exec();
}

void VMWidget::showMenu(const QPoint &point)
{
    if(m_vm.status == RUNING)
    {
        operAction->setText(tr("关机"));
    }else
        operAction->setText(tr("开机"));
    m_menu->exec(mapToGlobal(point));
}

void VMWidget::setStyleSheetByStatus()
{
    if(m_vm.status == RUNING)
    {
        setStyleSheet(" QPushButton {  \
                          border: none; \
                          border-radius: 0px;   \
                          color:white;  \
                          font-weight:bold; \
                          image: url(:/new/index/run_namal);   \
                      } \
                      QPushButton:hover { \
                          border: 1px solid white; \
                          border-radius: 0px; \
                          color:white; \
                          font-weight:normal; \
                          image: url(:/new/index/run_namal_hover); \
                      } \
                      QPushButton:pressed { \
                          border: 1px solid white; \
                          border-radius: 0px; \
                          color:white; \
                          font-weight:bold; \
                          image: url(:/new/index/run_namal_press); \
                      }");
    }else
    {
        setStyleSheet(" QPushButton {  \
                  border: none; \
                  border-radius: 0px;   \
                  color:white;  \
                  font-weight:bold; \
                  image: url(:/new/index/down_namal);   \
              } \
              QPushButton:hover { \
                  border: 1px solid white; \
                  border-radius: 0px; \
                  color:white; \
                  font-weight:normal; \
                  image: url(:/new/index/down_namal_hover); \
              } \
              QPushButton:pressed { \
                  border: 1px solid white; \
                  border-radius: 0px; \
                  color:white; \
                  font-weight:bold; \
                  image: url(:/new/index/down_namal_press); \
              }");
    }

}

VMWidget::~VMWidget()
{
    delete ui;
}

void VMWidget::on_VMButton_clicked()
{
    emit emitData(m_vm);
}
