#ifndef VMWIDGET_H
#define VMWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "common.h"
namespace Ui {
class VMWidget;
}

class VMWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VMWidget(VM_CONFIG &vm, QWidget *parent = 0);
    ~VMWidget();
signals:
    void emitData(VM_CONFIG vm);
private slots:
    void on_VMButton_clicked();

private:
    Ui::VMWidget *ui;
    VM_CONFIG &m_vm;
};

#endif // VMWIDGET_H
