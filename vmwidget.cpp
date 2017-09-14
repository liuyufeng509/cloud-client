#include "vmwidget.h"
#include "ui_vmwidget.h"

VMWidget::VMWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VMWidget)
{
    ui->setupUi(this);
}

QPushButton * VMWidget::GetButton() const
{
    return ui->VMButton;
}

QLabel *VMWidget::GetLabel() const
{
    return ui->label_vmName;
}

VMWidget::~VMWidget()
{
    delete ui;
}
