#include "qmessagehandles.h"
#include "gwsdk.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include<QtDebug>

QMessageHandles* QMessageHandles::s_inst = NULL;


QMessageHandles* QMessageHandles::instance()
{
    if(NULL == s_inst)
    {
        QMessageHandles* temp = s_inst;
        if(NULL == temp)
        {
            temp = new QMessageHandles();
            s_inst = temp;
        }
    }
    return s_inst;
}

QMessageHandles::QMessageHandles(QObject *parent) : QObject(parent)
{

}

void QMessageHandles::ReceiveMassage(const uint dwContext, const uint evtID, const QString param)
{
    QStringList sl ;
    switch (evtID) {
        case EVENTID_USER_LOGINED://登录结果
        {
            //evtID:234, szPraram:{"rspID":234,"rspMsg":{"username":"kaifatest"}}
            QStringList sl ;

            QJsonParseError err_rpt;
            QJsonDocument  root_Doc = QJsonDocument::fromJson(param.toLocal8Bit(), &err_rpt);//字符串格式化为JSON
            if(err_rpt.error != QJsonParseError::NoError)
            {
                sl<< "登录结果JSON格式错误！";
                emit loginReady(false,"登录结果JSON格式错误！");
            }else {
                QJsonObject root_Obj = root_Doc.object();
                QJsonObject rspMsg_Value = root_Obj.value("rspMsg").toObject();
                sl<<"用户："+rspMsg_Value.value("username").toString()+"登录成功";
                emit DeskTipMessage(sl,1);
                emit loginReady(true,"");
            }
        }
        break;

        case EVENTID_SERVICELIST://服务（业务）列表
        {
            //evtID:208, szPraram:{"rspID":208,"rspMsg":{"ServiceCount":5,"ServiceList":[{"AccessType":1,"DisplayName":"ftp_5","ID":5,"ServType":3},{"AccessType":1,"DisplayName":"统一运维平台","ID":1,"ServType":1},{"AccessType":1,"DisplayName":"重庆地铁智能运维系统","ID":2,"ServType":1},{"AccessType":1,"DisplayName":"ftp_12","ID":6,"ServType":3},{"AccessType":1,"DisplayName":"nginx代理-重庆","ID":3,"ServType":1}]}}
            emit servicesList(param);
        }
        break;

        case EVENTID_GroupListEnd: //服务的ip等信息
        {
            emit UpdateServciesList(param);
        }
        break;

        case EVENTID_PROXY_STARTED://代理服务已启动
        {
            //evtID:234, szPraram:{"rspID":234,"rspMsg":{"username":"kaifatest"}}
            QJsonParseError err_rpt;
            QJsonDocument  root_Doc = QJsonDocument::fromJson(param.toLocal8Bit(), &err_rpt);//字符串格式化为JSON
            if(err_rpt.error != QJsonParseError::NoError)
            {
                sl<< "登录结果JSON格式错误！";
                emit loginReady(false,"登录结果JSON格式错误！");
            }else {
                QJsonObject root_Obj = root_Doc.object();
                QJsonObject rspMsg_Value = root_Obj.value("rspMsg").toObject();
                sl<<"用户："+rspMsg_Value.value("username").toString()+"登录成功";
                emit loginReady(true,"");
            }
        }
        break;

        case EVENTID_NC_STARTED://NC服务已启动
        {
            //evtID:234, szPraram:{"rspID":234,"rspMsg":{"username":"kaifatest"}}
            QJsonParseError err_rpt;
            QJsonDocument  root_Doc = QJsonDocument::fromJson(param.toLocal8Bit(), &err_rpt);//字符串格式化为JSON
            if(err_rpt.error != QJsonParseError::NoError)
            {
                sl<< "登录结果JSON格式错误！";
                emit loginReady(false,"登录结果JSON格式错误！");
            }else {
                QJsonObject root_Obj = root_Doc.object();
                QJsonObject rspMsg_Value = root_Obj.value("rspMsg").toObject();
                sl<<"用户："+rspMsg_Value.value("username").toString()+"登录成功";
                emit loginReady(true,"");
            }
        }
        break;
    }


}
