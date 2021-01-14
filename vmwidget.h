#ifndef VMWIDGET_H
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

class VMWidget : public QFrame
{
    Q_OBJECT

public:
    explicit VMWidget(Service &vm, QWidget *parent = 0);
    ~VMWidget();
    void setStyleSheetByStatus();

    void setSvrIP(QString ip){worker->setSvrIP(ip);}
    void setUserInfo(UserInfo usr){worker->setUserInfo(usr);}

protected:
    //void paintEvent(QPaintEvent *);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
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
    bool show_menu;
};

#endif // VMWIDGET_H
