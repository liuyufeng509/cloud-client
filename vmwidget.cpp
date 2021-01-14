#include "vmwidget.h"
#include "ui_vmwidget.h"
#include "detaildialog.h"
#include "homewindow.h"
#include<unistd.h>
#include<QIcon>
#include<QFile>
#include "setapprelationdialog.h"
#include<QStandardPaths>
#include<QStyleOption>
VMWidget::VMWidget(Service &vm,QWidget *parent) :
    QFrame(parent),
    m_vm(vm),
    prt(parent),
    ui(new Ui::VMWidget),
    show_menu(false)
{
    ui->setupUi(this);

    setStyleSheetByStatus();
    ui->label_vmName->setText(m_vm.displayName);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_menu = new QMenu(this);
    setAction = new QAction( QIcon(":/new/vpn/set"),"设置启动程序",this);
    exportAction = new QAction(QIcon(":/new/vpn/export"),"创建快捷方式",this);

    connect(setAction, SIGNAL(triggered()), this, SLOT(operateActionSlot()));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(detailActionSlot()));

    m_menu->addAction(setAction);
    m_menu->addAction(exportAction);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showMenu(const QPoint&)));

    worker = new Worker;

}

void VMWidget::operateActionSlot()
{
    //startup or shutdown
    SetAppRelationDialog dlg(m_vm,this);
    dlg.exec();

}


// If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
//void VMWidget::paintEvent(QPaintEvent *)
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

//       QPainter p(this);
//       p.setPen(Qt::NoPen);
//       p.setBrush(Qt::white);
//       p.drawRect(rect());

//}

void VMWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_vm.islocationImage)
        ui->VMButton->setStyleSheet("border-image: url("+m_vm.imageUrl_hover+")");
    else
    ui->VMButton->setStyleSheet("border-image: url("+m_vm.imageUrl+"_hover)");
    QWidget::mouseReleaseEvent(event);
}

void VMWidget::mousePressEvent(QMouseEvent *event)
{
    if(m_vm.islocationImage)
        ui->VMButton->setStyleSheet("border-image: url("+m_vm.imageUrl_press+")");
    else
    ui->VMButton->setStyleSheet("border-image: url("+m_vm.imageUrl+"_press)");
    setStyleSheetByStatus();
    on_VMButton_clicked();
    QWidget::mousePressEvent(event);
}

void VMWidget::detailActionSlot()
{
    //快捷方式
    /* *.url
     * [InternetShortcut]
URL=http://Kennytian.cnblogs.com
     * */
    QString location = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString filename = location+"/"+m_vm.displayName+".url";
    QFile file(filename);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream ts(&file);
        ts<<"[InternetShortcut]"<<"\n";
        ts<<"URL="+m_vm.url<<"\n";
        ts<<"IconIndex=0"<<"\n";
        ts<<"HotKey=0"<<"\n";
        ts<<"IDList="<<"\n";
        ts<<"IconFile="+qApp->applicationFilePath()<<"\n";
        ts<<"[{000214A0-0000-0000-C000-000000000046}]"<<"\n";
        ts<<"Prop3=19,1"<<"\n";
        file.close();
    }
}

void VMWidget::showMenu(const QPoint &point)
{
    show_menu = true;
    m_menu->exec(mapToGlobal(point));
}

void VMWidget::enterEvent(QEvent *event)
{
    ui->label_vmName->setStyleSheet("font: 10pt \"微软雅黑\";"
                                    "color: white;");
    if(m_vm.islocationImage)
        ui->VMButton->setStyleSheet("border-image: url("+m_vm.imageUrl_hover+")");
    else
    ui->VMButton->setStyleSheet("border-image: url("+m_vm.imageUrl+"_hover)");
    QWidget::enterEvent(event);
}
void VMWidget::leaveEvent(QEvent *event)
{
    if(!show_menu)
        ui->label_vmName->setStyleSheet("font: 10pt \"微软雅黑\";"
                                        "color: black;");
    show_menu = false;
    ui->VMButton->setStyleSheet("border-image: url("+m_vm.imageUrl+")");;
    QWidget::leaveEvent(event);
}

void VMWidget::setStyleSheetByStatus()
{
    m_vm.getUrlandIcon();
    if(m_vm.islocationImage)
    {
        setStyleSheet(
                    "QFrame{"
                    "background-color: white;"
                    "border-radius: 5px;"
                    "}"
                    "QFrame:hover{"
                    "background-color: rgb(86,137,247);"
                    "border-radius: 5px;"
                    "}"
                    "QLabel{"
                    "font: 10pt \"华文楷体\";"
                    "color: rgb(0, 0, 0);"
                    "background:transparent;"
                    "}"
                    "QPushButton {  "
                       "   color:white;  "
                        "    border: 1px solid white; "
                         "   border-radius: 33px; "
                          "font-weight:bold; "
                         " border-image: url("+m_vm.imageUrl+");   "
                      "} "
                     "QPushButton:hover {  "
                        "   color:white;  "
                         "    border: 1px solid white; "
                          "   border-radius: 33px; "
                           "font-weight:bold; "
                          " border-image: url("+m_vm.imageUrl_hover+");   "
                       "} "
                     "QPushButton:pressed {  "
                        "   color:white;  "
                         "    border: 1px solid white; "
                          "   border-radius: 33px; "
                           "font-weight:bold; "
                          " border-image: url("+m_vm.imageUrl_press+");   "
                       "} "
                    );
    }else
    setStyleSheet(
                "QFrame{"
                "background-color: white;"
                "border-radius: 5px;"
                "}"
                "QFrame:hover{"
                "background-color: rgb(86,137,247);"
                "border-radius: 5px;"
                "}"
                "QLabel{"
                "font: 10pt \"华文楷体\";"
                "color: rgb(0, 0, 0);"
                "background:transparent;"
                "}"
                "QPushButton {  "
                   "   color:white;  "
                    "    border: 1px solid white; "
                     "   border-radius: 33px; "
                      "font-weight:bold; "
                     " border-image: url("+m_vm.imageUrl+");   "
                  "} "
                 "QPushButton:hover {  "
                    "   color:white;  "
                     "    border: 1px solid white; "
                      "   border-radius: 33px; "
                       "font-weight:bold; "
                      " border-image: url("+m_vm.imageUrl+"_hover);   "
                   "} "
                 "QPushButton:pressed {  "
                    "   color:white;  "
                     "    border: 1px solid white; "
                      "   border-radius: 33px; "
                       "font-weight:bold; "
                      " border-image: url("+m_vm.imageUrl+"_press);   "
                   "} "
                );
}

VMWidget::~VMWidget()
{
    delete ui;
}

void VMWidget::on_VMButton_clicked()
{
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

            if(s==m_vm)
            {
                m_vm.appUrl = s.appUrl;
                m_vm.arg = s.arg;
            }
        }
        file.close();
    }
    emit emitData(m_vm);
}
