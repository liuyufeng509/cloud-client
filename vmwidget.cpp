#include "vmwidget.h"
#include "ui_vmwidget.h"


VMWidget::VMWidget(VM_CONFIG &vm,QWidget *parent) :
    QWidget(parent),
    m_vm(vm),
    ui(new Ui::VMWidget)
{
    ui->setupUi(this);

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

    ui->label_vmName->setText(m_vm.name);

}

VMWidget::~VMWidget()
{
    delete ui;
}

void VMWidget::on_VMButton_clicked()
{
    emit emitData(m_vm);
}
