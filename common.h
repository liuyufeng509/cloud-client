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

struct Service
{
    int accessType;
    QString displayName;
    int id;
    int servType;
    QString ip;
    QString url;
    QString imageUrl;
    QIcon icon;
    QString appUrl;
    QString arg;

    bool operator== (const Service& ar) const
    { return accessType== ar.accessType && displayName==ar.displayName && id==ar.id && servType==ar.servType;}

    void getUrlandIcon()
    {
        switch (servType) {
        case 1:
            url = "http://"+ip+":80";
            imageUrl = ":/new/vpn/services/http";
            icon.addFile(":/new/vpn/services/http");

            return;
        case 2:
            url = "https://"+ip+":443";
            imageUrl = ":/new/vpn/services/https";
            icon.addFile(":/new/vpn/services/https");
            return;
        case 3:
            url = "ftp://"+ip+":21";
            imageUrl = ":/new/vpn/services/ftp";
            icon.addFile(":/new/vpn/services/ftp");
            return;
        case 4:
            url = "telnet "+ip +":23";
            return;
        case 5:
            url = "ssh "+ip +":22";
            return;
        case 14:
            url = "tftp://"+ip+":69";
            return;

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

