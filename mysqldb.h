#ifndef MysqlDb_H
#define MysqlDb_H
#include "common.h"
#include<QSqlDatabase>
class MysqlDb
{
public:
    static MysqlDb *instance();
    static MysqlDb *s_inst;

    bool isOpen() const;

    ~MysqlDb();


    bool createTables();

    QList<AppIcon> getAllAppIcons();

private:
    MysqlDb();

    QSqlDatabase m_db;
};

#endif // MysqlDb_H
