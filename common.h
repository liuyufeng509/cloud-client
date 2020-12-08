#ifndef COMMON
#define COMMON
#include<QString>
#include<QVector>
#include<QStringList>
#include <QtDebug>
#include<QMessageBox>
#include"gwsdk.h"
#include <Windows.h>

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

