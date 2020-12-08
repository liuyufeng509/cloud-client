#include "common.h"
#include<QCoreApplication>
#include "qmessagehandles.h"
HINSTANCE hSDK = nullptr;
FUNCLoginSslVpnEx funcLoginSslVpnEx = nullptr;
FUNCLogoutSslVpn funcLogoutSslVpn = nullptr;
FUNCLoginSslVpnByGMKeyEx funcLoginSslVpnByGMKeyEx = nullptr;
FUNCQueryTcpServiceStatus funcQueryTcpServiceStatus = nullptr;
FUNCSetGWCertFinger funcSetGWCertFinger = nullptr;

bool loadSDK()
{
    if (hSDK) return false;

    QString path = QCoreApplication::applicationDirPath()+"/gwsdk.dll";
    hSDK = ::LoadLibrary(path.toStdWString().c_str());
    if (hSDK==nullptr)
    {
        qDebug()<<"加载dll失败:"<<::GetLastError();
        return false;
    }
    funcLoginSslVpnEx = (FUNCLoginSslVpnEx) ::GetProcAddress(hSDK, "LoginSslVpnEx");
    funcLogoutSslVpn = (FUNCLogoutSslVpn) ::GetProcAddress(hSDK, "LogoutSslVpn");
    funcLoginSslVpnByGMKeyEx = (FUNCLoginSslVpnByGMKeyEx) ::GetProcAddress(hSDK, "LoginSslVpnByGMKeyEx");
    funcQueryTcpServiceStatus = (FUNCQueryTcpServiceStatus) ::GetProcAddress(hSDK, "QueryTcpServiceStatus");
    funcSetGWCertFinger = (FUNCSetGWCertFinger) ::GetProcAddress(hSDK, "SetGWCertFinger");
    return true;
}


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
    const char* szParam)
{
    qDebug()<<"=======================================dwContext:"<<dwContext<<", evtID:"<<
              evtID<<", szPraram:"<<QString::fromLocal8Bit(szParam);

    QMessageHandles::instance()->ReceiveMassage(dwContext, evtID, QString::fromLocal8Bit(szParam));
}


QString GetCmdRes(QString cmd)
{
    FILE *pf;
    char buff[1000]={};
    QByteArray res ;
    pf = popen(cmd.toStdString().c_str(), "r");
    //fread(buff,sizeof(buff),sizeof(buff),pf);
    while(!feof(pf))
    {
        fread(buff,sizeof(buff)-1,1,pf);
        res += QByteArray(buff);
        //qDebug()<<buff;
        memset(buff,0,sizeof(buff));
    }

    pclose(pf);
    //qDebug()<<res;
    QString resStr(res);
    return resStr;
}
