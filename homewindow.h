#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QWidget>
#include <QDateTime>
#include "common.h"
#include "mytitlebar.h"
#include <QMainWindow>
#include "worker.h"
#include "waitdialog.h"
#include "httpdownload/httppicdownload.h"
#include<QThread>
namespace Ui {
class HomeWindow;
}

class QButtonGroup;
class QTranslator;
class QSettings;
class QMenu;
class QFlowLayout;

class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent = 0);
    ~HomeWindow();
    void initLanguage();

    void updateViewUI();
    void updatetableUI();

    void clearLayout(QLayout *layout);

signals:
    void getAllInfo(QVector<VM_CONFIG> *vmArr, QString &vms);

protected:
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();

    void openVm(VM_CONFIG vm);
    void openVmOfTable(int row, int column);
    void handleGetAllInfoRes(bool success);
    void on_freshButton_clicked();

    void on_logoutButton_clicked();

    void tabChanged(int );
    void on_tableWidget_customContextMenuRequested(QPoint pos);
    void detailActionSlot();
    void operateActionSlot();

    void on_updateButton_clicked();

    void addImage(QString path, int counter);

private:
    void moveToCenter();

    void initTitleBar();

    void waitDiaogAppear();
    void waitDialogAccept();
private:
    Ui::HomeWindow *ui;

protected:
    MyTitleBar* m_titleBar;
private:

    QString serverIp;
    UserInfo m_userInfo;
    QString vms;

    WaitDialog *waitD;
    QThread workerThread;
    Worker *worker;


    //table context menu
    QMenu *m_menu;
    QAction *operAction;
    QAction *detailAction;


    //http图片下载
    HttpPicDownLoad *m_picDown;
public:
    QVector<VM_CONFIG> vmArray;


    int minWidth=700;
    int minHeight=500;
    QFlowLayout *m_flowLayout;
};


#endif // HOMEWINDOW_H
