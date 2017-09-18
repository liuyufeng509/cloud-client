#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include"common.h"
class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);
//    static Worker*  getInstance()
//   {
//       if(m_pInstance == NULL)
//       {
//           m_pInstance = new SecFunClass();
//       }
//       return m_pInstance;
//   }
    void setSvrIP(QString ip){serverIP = ip;}

private:
    bool Login(UserInfo  &userInfo);

signals:
    void loginReady(bool success);
public slots:
    void doLogin(UserInfo &userinfo);
private:

    //static Worker *m_pInstance;
    QString serverIP;
};

#endif // WORKER_H
