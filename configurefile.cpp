#include "configurefile.h"
#include <assert.h>

ConfigureFile::ConfigureFile(QString appName, QString configFileName)
{
    HomeDir = getenv("HOME");

    QString settingFileDir = QString("%1/.config/%2").arg(HomeDir).arg(appName);

    system(QString("mkdir -p "+ settingFileDir).toLatin1().data());

    QString settingFilePath = QString("%1/%2").arg(settingFileDir)
                                              .arg(configFileName);

    settings = new QSettings(settingFilePath, QSettings::IniFormat);

}

void ConfigureFile::SaveConfigure(QString name, QString value)
{
    assert(settings != NULL);
    settings->setValue(name, value);
}

QString ConfigureFile::LoadConfigure(QString name)
{
    assert(settings != NULL);

    QVariant var = settings->value(name);

    return var.toString();
}
