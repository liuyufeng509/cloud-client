﻿#ifndef VMWIDGET_H
#define VMWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "common.h"
#include<QAction>
#include <QMenu>
#include "worker.h"

namespace Ui {
class VMWidget;
}

class VMWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VMWidget(Service &vm, QWidget *parent = 0);
    ~VMWidget();
    void setStyleSheetByStatus();

    void setSvrIP(QString ip){worker->setSvrIP(ip);}
    void setUserInfo(UserInfo usr){worker->setUserInfo(usr);}
signals:
    void emitData(Service vm);

private slots:
    void on_VMButton_clicked();
    void showMenu(const QPoint &point);
    void operateActionSlot();
    void detailActionSlot();

private:
    Ui::VMWidget *ui;
    Service &m_vm;

    //context menu
    QMenu *m_menu;
    QAction *setAction;
    QAction *exportAction;
    Worker *worker;

    QWidget *prt;
};

#endif // VMWIDGET_H
