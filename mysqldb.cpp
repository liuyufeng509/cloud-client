#include "MysqlDb.h"
#include<QtDebug>
#include<QCoreApplication>
#include<QSqlQuery>
#include<QSqlError>
#include<QFile>
#include <QBuffer>
#include<QImageReader>
#include <QApplication>
#include<QSettings>

MysqlDb* MysqlDb::s_inst = NULL;

MysqlDb* MysqlDb::instance()
{
    if(NULL == s_inst)
    {
        MysqlDb* temp = s_inst;
        if(NULL == temp)
        {
            temp = new MysqlDb();
            s_inst = temp;
        }
    }
    return s_inst;
}

MysqlDb::MysqlDb()
{
    QString path = QApplication::applicationDirPath();
    path = path + QString("/")+ QString("config.ini");
    QSettings settings(path, QSettings::IniFormat);
    QString ip = settings.value("mysql/ip",'localhost').toString();
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName(ip);
    m_db.setDatabaseName("vpnclient");
    m_db.setUserName("root");
    m_db.setPassword("vpnclient");

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database fail"<<m_db.lastError();
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}


QList<AppIcon> MysqlDb::getAllAppIcons()
{
    QList<AppIcon> appicons;
    QString path = QCoreApplication::applicationDirPath()+"/";
    QString sql = "select * from appicon";
    QSqlQuery query(m_db);
    if(!query.exec(sql))
    {
    qDebug()<<"select "<<query.lastError().text();
    }
    while(query.next())
    {
        AppIcon appIcon;
        appIcon.appname = query.value(0).toString();
        QByteArray bytes = query.value(1).toByteArray();
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::ReadOnly);
        QImageReader reader(&buffer,"PNG");
        QImage img = reader.read();
        img.save(path+appIcon.appname+".png");
        appIcon.icon = path+appIcon.appname+".png";
        QByteArray bytes2 = query.value(2).toByteArray();
        QBuffer buffer2(&bytes2);
        buffer2.open(QIODevice::ReadOnly);
        QImageReader reader2(&buffer2,"PNG");
        QImage img2 = reader2.read();
        img2.save(path+appIcon.appname+"_press.png");
        appIcon.icon_press = path+appIcon.appname+"_press.png";
        QByteArray bytes3 = query.value(3).toByteArray();
        QBuffer buffer3(&bytes3);
        buffer3.open(QIODevice::ReadOnly);
        QImageReader reader3(&buffer3,"PNG");
        QImage img3 = reader3.read();
        img3.save(path+appIcon.appname+"_hover.png");
        appIcon.icon_hover = path+appIcon.appname+"_hover.png";

        appicons.append(appIcon);
    }
    return appicons;
}

bool MysqlDb::isOpen() const
{
    return m_db.isOpen();
}

//bool MysqlDb::removeMapInfo(QString condition)
//{
//    QString sql = "delete from mapinfo ";
//    if(condition.trimmed().length()!=0)
//        sql+="where "+condition;
//    bool success=false;
//    QSqlQuery queryDelete;
//    queryDelete.prepare(sql);
//    success = queryDelete.exec();

//    if(!success)
//    {
//        qDebug() << "remove person failed: " << queryDelete.lastError();
//    }
//    m_db.commit();
//    return success;
//}

//QList<MapInfo> MysqlDb::getMapInfos(QString condition)
//{
//    QList<MapInfo> mapinfos;

//    QString sql = "select ebGraphic,  qetName,qetType, qetPath from mapinfo ";
//    if(condition.trimmed().length()!=0)
//        sql+="where "+condition;

//    QSqlQuery query(sql);
//    while (query.next()) {
//        MapInfo mapinfo;
//        mapinfo.ebGraphic = query.value(0).toString();
//        mapinfo.qetName = query.value(1).toString();
//        mapinfo.qetType = query.value(2).toString();
//        mapinfo.qetPath = query.value(3).toString();
//        mapinfos.append(mapinfo);
//    }
//    return mapinfos;
//}

MysqlDb::~MysqlDb()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

//bool MysqlDb::mapInfoExists(MapInfo mapInfo)
//{
//    return false;
//}

//bool MysqlDb::insertIntoMapInfo(QList<MapInfo> datas)
//{
//    if(datas.isEmpty())
//        return true;

//    bool success = true;
//    QList<MapInfo> insert_datas;
//    QList<MapInfo> update_datas;
//    //insert
//    for(int i=0;i<datas.size();i++)
//    {
//        QList<MapInfo> tmp = getMapInfos(" ebGraphic=\'"+datas[i].ebGraphic+"\'");
//        if(tmp.isEmpty())
//            insert_datas.append(datas[i]);
//        else
//            update_datas.append(datas[i]);
//    }
//    QString sql_insert = "insert into mapinfo values ";
//    QString sql_insert_values="";
//    for(int i=0; i<insert_datas.size();i++)
//    {
//        sql_insert_values +="(";
//        sql_insert_values += "\'" + insert_datas[i].ebGraphic + "\',\'" +
//                        insert_datas[i].qetName + "\',\'" + insert_datas[i].qetType + "\',\'" + insert_datas[i].qetPath + "\'";
//        sql_insert_values += "),";
//    }
//    sql_insert_values = sql_insert_values.left(sql_insert_values.size()-1);
//    QString sql_todo = sql_insert+sql_insert_values;

//    if (!insert_datas.isEmpty())
//    {
//        QSqlQuery queryAdd;
//        queryAdd.prepare(sql_todo);
//        if(queryAdd.exec())
//        {
//            //success = true;
//        }
//        else
//        {
//            qDebug() << "add data failed: " << queryAdd.lastError();
//            success = false;
//        }
//    }


//    //update
//    for(int i=0;i<update_datas.size();i++)
//    {
//        QString sql_update ="update mapinfo set qetName=\'"+update_datas[i].qetName+"\',qetType=\'"+update_datas[i].qetType+
//                "\',qetPath=\'"+update_datas[i].qetPath+"\' where ebGraphic=\'"+update_datas[i].ebGraphic+"\'" ;
//        QSqlQuery queryUpdate;
//        queryUpdate.prepare(sql_update);
//        if(queryUpdate.exec())
//        {
//            //success = true;
//        }
//        else
//        {
//            qDebug() << "update data failed: " << queryUpdate.lastError();
//            success = false;
//        }
//    }

//    if(success)
//        m_db.commit();
//    else {
//        m_db.rollback();
//    }

//    return success;
//}

bool MysqlDb::createTables()
{
//    if(!m_db.isOpen())
//    {
//        qDebug()<<"createTables: db is not open";
//        return false;
//    }

//    QSqlQuery query;
//    query.prepare("create table if not exists train_connector_excel_path("
//                  "path VARCHAR (300) PRIMARY KEY)");
//    query.exec();
//    if(query.lastError().type()!=QSqlError::NoError)
//    {
//        qDebug()<<"create train_connector_excel_path failed:"<<query.lastError().text();
//    }

//    query.prepare("create table if not exists circle_terminals_excels_path("
//                  "path VARCHAR (300) PRIMARY KEY)");
//    query.exec();
//    if(query.lastError().type()!=QSqlError::NoError)
//    {
//        qDebug()<<"create circle_terminals_excels_path failed:"<<query.lastError().text();
//    }


//    query.prepare("create table if not exists fileinfo("
//                  "path     VARCHAR (300),"
//                  "filename VARCHAR (300),"
//                  "train    VARCHAR (20),"
//                  "[group]  VARCHAR (20),"
//                  "page     VARCHAR (20)"
//                  ")");
//    query.exec();
//    if(query.lastError().type()!=QSqlError::NoError)
//    {
//        qDebug()<<"create fileinfo failed:"<<query.lastError().text();
//    }

    return true;
}

//bool MysqlDb::saveDxfFilesToDB(QList<FileInfo> &files)
//{
//    QString path = QCoreApplication::applicationDirPath()+"/tmpdb";
//    QFile file(path);
//    if(!file.open(QFile::WriteOnly))
//    {
//        return false;
//    }
//    QTextStream To_Dxf(&file);
//    for(int i=0;i<files.size();i++)
//    {
//        To_Dxf<<files[i].path<<"$"<<files[i].filename
//             <<"$"<<files[i].train<<"$"<<files[i].group<<"$"<<files[i].page<<"\n";
//    }
//    file.close();


////    if(!m_db.isOpen())
////    {
////        qDebug()<<"saveDxfFilesToDB: db is not open";
////        return false;
////    }

////    QSqlQuery query;
////    query.prepare("delete from fileinfo");
////    if(!query.exec())
////    {
////        qDebug()<<"delete from fileinfo failed:"<<query.lastError();
////    }
////    m_db.transaction();
////    query.prepare("insert into fileinfo values(?,?,?,?,?)");
////    QVariantList paths, filenames, trains, groups, pages;
////    for(int i=0;i<files.size();i++)
////    {
////        paths<<files[i].path;
////        filenames<<files[i].filename;
////        trains<<files[i].train;
////        groups<<files[i].group;
////        pages<<files[i].page;
////    }

////    query.addBindValue(paths);
////    query.addBindValue(filenames);
////    query.addBindValue(trains);
////    query.addBindValue(groups);
////    query.addBindValue(pages);

////    if(!query.execBatch())
////    {
////        qDebug()<<"insert into fileinfo failed:"<<query.lastError();
////        return false;
////    }
////    m_db.commit();
//    return true;
//}
