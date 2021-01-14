#ifndef COMMON
#define COMMON
#include<QString>
#include<QVector>
#include<QStringList>
#include <QtDebug>
#include<QMessageBox>
#include"gwsdk.h"
#include <Windows.h>
#include <QIcon>

struct  UserInfo//登陆窗口结构体
{
    QString ip;
    QString port;
    QString uname;
    QString pwd;
    QString otp;
};

struct AppIcon{
    QString appname;
    QString icon;
    QString icon_press;
    QString icon_hover;
};

struct Service
{
    int accessType;
    QString displayName;
    int id;
    int servType;
    QString ip;
    QString url;
    QString imageUrl;
    QString imageUrl_press;
    QString imageUrl_hover;
    bool islocationImage;
    QIcon icon;
    QString appUrl;
    QString arg;
    QList<AppIcon> appicons;

    bool operator== (const Service& ar) const
    { return accessType== ar.accessType && displayName==ar.displayName && id==ar.id && servType==ar.servType;}

    void getUrlandIcon()
    {
        islocationImage = false;
        switch (servType) {
        case 1:
            url = "http://"+ip+":80";
            imageUrl = ":/new/vpn/services/http";
            icon.addFile(":/new/vpn/services/http");

            break;
        case 2:
            url = "https://"+ip+":443";
            imageUrl = ":/new/vpn/services/https";
            icon.addFile(":/new/vpn/services/https");
            break;
        case 3:
            url = "ftp://"+ip+":21";
            imageUrl = ":/new/vpn/services/ftp";
            icon.addFile(":/new/vpn/services/ftp");
            break;
        case 4:
            url = "telnet "+ip +":23";
            return;
        case 5:
            url = "ssh "+ip +":22";
            break;
        case 14:
            url = "tftp://"+ip+":69";
            break;
        default:
            imageUrl = ":/new/vpn/services/default";
            icon.addFile(":/new/vpn/services/defualt");

        }

        for(int i=0;i<appicons.size();i++)
        {
            if(appicons[i].appname == displayName)
            {
                islocationImage = true;
                imageUrl = appicons[i].icon;
                imageUrl_hover = appicons[i].icon_hover;
                imageUrl_press = appicons[i].icon_press;
                break;
            }
        }
    }
};

enum STAT
{
    SHUTDOWN=0,
    RUNING,
};

struct Addr
{
    QString ip;
    QString mac;
    QString type;
};

struct VM_CONFIG{
    QString name;
    QString vid;
    QString ip;
    int port;
    STAT status;
    QString created;
    QVector<Addr> addrs;
};



typedef QVector<VM_CONFIG>   VMVECTOR;//TE策略列表类型

QString GetCmdRes(QString cmd);

extern const qreal WIDTH_TO_HEIGHT;


//SDK 相关
extern HINSTANCE hSDK;
extern FUNCLoginSslVpnEx funcLoginSslVpnEx;
extern FUNCLogoutSslVpn funcLogoutSslVpn;
extern FUNCLoginSslVpnByGMKeyEx funcLoginSslVpnByGMKeyEx;
extern FUNCQueryTcpServiceStatus funcQueryTcpServiceStatus;
extern FUNCSetGWCertFinger funcSetGWCertFinger;

bool loadSDK();//加载SDK

/*
FUNCEventCB: LoginSslVpnEx函数使用的回调函数定义
参数说明：
dwContext
[in]unsigned int，LoginSslVpnEx调用时传入的上下文参数
evtID
[in]unsigned int，事件的ID，参照EVENTID_*的定义
szParam
[in]char*，事件相关参数，各事件参数不同，JSON格式
*/
void __stdcall funcEventCB(
    unsigned int dwContext,
    unsigned int evtID,
    const char* szParam);

#endif // COMMON

