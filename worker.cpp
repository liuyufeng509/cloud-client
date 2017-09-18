#include "worker.h"

//Worker* Worker::m_pInstance = NULL;
Worker::Worker(QObject *parent) : QObject(parent)
{

}
//login
void Worker::doLogin(UserInfo &userinfo)
{
        emit loginReady(Login(userinfo));
}

bool Worker::Login(UserInfo  &userInfo)
{
    QString cmd = "/root/getLoginStatus.py "+userInfo.uname+" "+userInfo.pwd+" "+serverIP + " 2>&1";
    QString res = GetCmdRes(cmd).trimmed();
    QStringList list = res.split('\n');
    if(list.size()<1)
    {
        qDebug()<<tr("Login failed: printed info nums less than 2");
        return false;
    }
    if(list.first()=="login success")
    {
        userInfo.uid = list.last();
        return true;
    }else
    {
        userInfo.uid = "";
        return false;
    }
}
