#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QWidget>
#include <QDateTime>
#include "common.h"
#include "mytitlebar.h"
#include <QMainWindow>
namespace Ui {
class HomeWindow;
}

class QButtonGroup;
class QTranslator;
class QSettings;
class QMenu;


class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent = 0);
    ~HomeWindow();
    void initLanguage();
    bool getVMs();
    bool parseVMs(QByteArray &ba);

    bool getVMsIpPort();
    bool parseVMsIpPort(QString output);

    bool getVMsInfo();
    bool parseVMsInfo(QByteArray &ba);

    void updateUI();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();

    void openVm(VM_CONFIG vm);

private:
    void moveToCenter();

    void initTitleBar();
private:
    Ui::HomeWindow *ui;

protected:
    MyTitleBar* m_titleBar;
private:

    QVector<VM_CONFIG> vmArray;
    QString serverIp;
    UserInfo m_userInfo;
    QString vms;
};


#endif // HOMEWINDOW_H
