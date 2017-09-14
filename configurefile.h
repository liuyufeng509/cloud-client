#ifndef CONFIGUREFILE_H
#define CONFIGUREFILE_H
#include <QString>
#include <QSemaphore>
#include <QSettings>

class ConfigureFile
{
public:
    ConfigureFile(QString appName, QString configFileName);
    ~ConfigureFile(){
        delete settings;
    }
    void SaveConfigure(QString name, QString value);

    QString LoadConfigure(QString name);
    bool isConfigured(QString name){
        return !LoadConfigure(name).isEmpty();

    }
    void DelConfigure(QString name)
    {
        settings->remove(name);
    }

private:
    QString HomeDir;
    QString AppName;
    QString ConfigFileName;
    QSettings *settings;
};

#endif // CONFIGUREFILE_H
