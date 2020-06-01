#include "homewindow.h"
#include <QApplication>
#include <QFile>
#include <QMutex>
#include <iostream>
using namespace std;
//日志生成
void LogMsgOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
 {
    // 加锁
       static QMutex mutex;
       mutex.lock();

       QByteArray localMsg = msg.toLocal8Bit();

       QString strMsg("");
       switch(type)
       {
       case QtDebugMsg:
           strMsg = QString("Debug:");
           break;
       case QtWarningMsg:
           strMsg = QString("Warning:");
           break;
       case QtCriticalMsg:
           strMsg = QString("Critical:");
           break;
       case QtFatalMsg:
           strMsg = QString("Fatal:");
           break;
       }

       // 设置输出信息格式
       QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
       QString strMessage = QString("%1  File:%2  Line:%3  Function:%4  DateTime:%5 Message:%6")
               .arg(strMsg).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime).arg(localMsg.constData());

       // 输出信息至文件中（读写、追加形式）
       QFile file("log.txt");
       file.open(QIODevice::ReadWrite | QIODevice::Append);
       qint64 len = file.size();
       if(len<1024*1024*100) //100M
       {
           QTextStream stream(&file);
           stream << strMessage << "\r\n";
           cout<<QString(localMsg.constData()).toStdString()<<endl;
       }else {
            file.remove();
       }

       file.flush();
       file.close();

       // 解锁
       mutex.unlock();
 }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //QFile *qss = new QFile(":/new/qss/style");
    QFile *qss = new QFile("F:/source/Qt/cloud-client/style/style.qss");
    qss->open(QFile::ReadOnly);
    QString styleSheet = qss->readAll();
    a.setStyleSheet(styleSheet);
    qss->close();
    qss->deleteLater();
    HomeWindow w;
    w.show();

    return a.exec();
}
