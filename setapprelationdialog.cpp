#include "setapprelationdialog.h"
#include "ui_setapprelationdialog.h"
#include<QFileDialog>
#include<QFileInfo>
#include<QSettings>
#include<QFile>
#include<QTextStream>
SetAppRelationDialog::SetAppRelationDialog(Service & serv,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetAppRelationDialog),
    m_serv(serv)
{
    ui->setupUi(this);
    QString path = QApplication::applicationDirPath();
    path = path + QString("/")+ QString("servs");
    QFile file(path);
    if(file.open(QFile::ReadOnly))
    {
        QString res = file.readAll();
        QStringList sl = res.split('\n');
        for(int i=0;i<sl.size();i++)
        {
            QStringList servs = sl[i].split('#');
            if(servs.size()!=6)
                continue;
            Service s;
            s.accessType = servs[0].toInt();
            s.displayName=servs[1];
            s.id = servs[2].toInt();
            s.servType = servs[3].toInt();
            s.appUrl=servs[4];
            s.arg = servs[5];
            m_servs.append(s);

            if(s==m_serv)
            {
                ui->appPathEdit->setText(s.appUrl);
                ui->paramEdit->setText(s.arg);
                m_serv.appUrl = s.appUrl;
                m_serv.arg = s.arg;
            }
        }
        file.close();
    }



}

SetAppRelationDialog::~SetAppRelationDialog()
{
    delete ui;
}

void SetAppRelationDialog::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "打开文件",".","*.exe");
    if(!filename.isEmpty())
    {
        appName = filename;
        ui->appPathEdit->setText(appName);
    }

}

void SetAppRelationDialog::on_saveButton_clicked()
{
    m_serv.appUrl = ui->appPathEdit->text();
    m_serv.arg = ui->paramEdit->text();
    QString path = QApplication::applicationDirPath();
    path = path + QString("/")+ QString("servs");
    QFile file(path);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream ts(&file);
        bool found=false;
        for(int i=0;i<m_servs.size();i++)
        {
            if(m_servs[i]==m_serv)
            {
                m_servs[i].appUrl = m_serv.appUrl;
                m_servs[i].arg = m_serv.arg;
                found = true;
            }

            ts<<QString::number(m_servs[i].accessType)+"#"+m_servs[i].displayName+"#"+QString::number(m_servs[i].id)
                +"#"+QString::number(m_servs[i].servType)+"#"+m_servs[i].appUrl+"#"+m_servs[i].arg<<'\n';
        }
        if(!found)
        {
            ts<<QString::number(m_serv.accessType)+"#"+m_serv.displayName+"#"+QString::number(m_serv.id)
                +"#"+QString::number(m_serv.servType)+"#"+m_serv.appUrl+"#"+m_serv.arg<<'\n';
        }

        file.close();
    }

    accept();
}

void SetAppRelationDialog::on_cancelButton_clicked()
{
    reject();
}
