#include "homewindow.h"
#include "ui_homewindow.h"
#include <QSettings>
#include <QFile>
#include <QApplication>
#include <QTranslator>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>

#include "simple_log.h"

#include "vmwidget.h"

#include <QtScript/QtScript>
#include <QMenu>
#include <QEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include "logindialog.h"
#include "3rd/json/json/json.h"
#include <qdebug.h>
#include <QTcpSocket>
#include <QHostAddress>

HomeWindow::HomeWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    moveToCenter();

    configFile                    = NULL;
    m_okClose                     = false;

    m_button_group = new QButtonGroup;
    connect(m_button_group, SIGNAL(buttonClicked (int)), this, SLOT(button_pressed_slot(int)) );

    configFile = new ConfigureFile(QCoreApplication::applicationName(), "config.ini");

    //初始化语言
    initLanguage();

    LoginDialog dia;
    if (QDialog::Accepted == dia.exec())
    {
        this->serverIp = dia.getServerIP();
        qDebug() << this->serverIp << endl;
    }
    else
    {
        exit(1);
    }

    refreshVM();
}

void HomeWindow::moveToCenter()
{
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
}

//设置样式表
void HomeWindow::setAppStyleSheet(const QString lang)
{
    QString styleFilePath;
    if(lang == "chinese")
    {
       styleFilePath = ":/style/style.css";
    }
    else if (lang == "english")
    {
        styleFilePath = ":/style/style_english.css";
    }

    QFile qss(styleFilePath);
    qss.open(QFile::ReadOnly);
    QString stylesheet = qss.readAll();
    qApp->setStyleSheet(stylesheet);
}

void HomeWindow::initLanguage()
{
    QString lang = configFile->LoadConfigure("translation/lang");
    m_translator = new QTranslator;

    if(lang != "english" && lang != "chinese")
    {
        lang = "chinese";
        configFile->SaveConfigure("translation/lang", lang);
    }

    lang_changed_slot(lang);
    m_curLang = lang;
}

HomeWindow::~HomeWindow()
{
    delete ui;

    delete configFile;

    delete m_button_group;

    delete m_translator;
}

void HomeWindow::changeEvent(QEvent *e)
{
   switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);//这里实现语言翻译器的更新。
       break;
    default:
       break;
    }
}

void HomeWindow::closeEvent(QCloseEvent * event){
    if(m_okClose){  //Filter Alt+F4
        event->accept();
        QWidget::closeEvent(event);
    }else{
        event->ignore();
        return;
    }
}

//添加快捷键
void HomeWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() == Qt::ControlModifier)
    {
            if(e->key() == Qt::Key_F10)
            {
                setWindowState(Qt::WindowFullScreen);
            }
            if(e->key() == Qt::Key_F11)
            {
                setWindowState(Qt::WindowMinimized);
            }

            if(e->key() == Qt::Key_F12)
            {
                setWindowState(Qt::WindowNoState);
            }
    }
    if(e ->modifiers() == Qt::Key_Meta)
    {
        return;
    }
    if(e ->modifiers() == Qt::AltModifier  && e ->key() == Qt::Key_Tab)
    {
        e->ignore();
    }
    else
    {
        QWidget::keyPressEvent(e);
    }
}

/*鼠标按下事件*/
void HomeWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_Drag = true;
        m_DragPosition = event->globalPos() - this->pos();
        event->accept();
    }
}

/*鼠标移动事件*/
void HomeWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_Drag && (event->buttons() && Qt::LeftButton)) {
        move(event->globalPos() - m_DragPosition);
        event->accept();
    }
}

/*鼠标释放事件*/
void HomeWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_Drag = false;
}

/*启动VDI客户端*/
void HomeWindow::button_pressed_slot(int index)
{
    QString lang;

    if (m_curLang == "english")
    {
        lang = "en_US.UTF-8";
    }
    else
    {
        lang = "zh_CN.UTF-8";
    }

    VM_CONFIG vm = vmArray[index];

    QStringList globalArgs = qApp->arguments();
    qDebug() << globalArgs;

    QString cmd = QString("%1 spice://%2:%3 --full-screen").arg(globalArgs[1]).arg(vm.ip).arg(vm.port);

    LOG(DEBUG_LEVEL, cmd.toStdString().c_str());

    system(cmd.toStdString().c_str());

    return;
}

//切换语言
void HomeWindow::lang_changed_slot(QString lang)
{
    if(lang == "chinese")
    {
        m_translator->load(":/lang/lang_zh_CN.qm");
        configFile->SaveConfigure("translation/lang","chinese");
        setenv("LANG", "zh_CN.UTF8", 1);
    }
    else if (lang == "english")
    {
        m_translator->load(":/lang/lang_en_US.qm");
        configFile->SaveConfigure("translation/lang","english");
        setenv("LANG", "en_US.UTF8", 1);
    }

    qApp->installTranslator(m_translator);
    this->setAppStyleSheet(lang);

    m_curLang = lang;
}

void HomeWindow::on_closePtn_clicked()
{
    this->close();
    exit(0);
}

void HomeWindow::refreshVM()
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress(this->serverIp), 59000);
    connect(socket, SIGNAL(readyRead()), this, SLOT(on_parseInstances()));
}

void HomeWindow::on_parseInstances()
{
    this->vmArray.clear();
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QString output = socket->readAll();

    QScriptEngine engine;
    QScriptValue sc = engine.evaluate("value=" + output);

    if( sc.isArray())
    {
        QScriptValueIterator it(sc);
        while(it.hasNext())
        {
            it.next();

            QScriptValueIterator i(it.value().property("instances"));
            while(i.hasNext())
            {
                i.next();
                VM_CONFIG vm;
                vm.name = i.value().property("name").toString();
                vm.ip = i.value().property("hostip").toString();
                vm.port = i.value().property("spiceport").toInt32();
                if(!vm.ip.isEmpty())
                {
                    qDebug() << vm.name << vm.ip << endl;
                    vmArray.append(vm);
                }
            }
        }
    }

    int vm_count = vmArray.length();
    int already_count = ui->gridLayout->count();
    if(vm_count < already_count)
    {
        for(int i=vm_count; i<already_count; i++)
        {
            QAbstractButton *b = m_button_group->button(i);
            m_button_group->removeButton(b);
            ui->gridLayout->removeItem(ui->gridLayout->itemAt(i));
        }
    }
    else
    {
        for(int i=already_count; i<vm_count; i++)
        {
            VMWidget *vm = new VMWidget(this);

            ui->gridLayout->addWidget(vm, i/4, i%4);
            QPushButton *b = vm->GetButton();

            m_button_group->addButton(b, i);
            b->setFocusPolicy(Qt::NoFocus);
        }
    }

    for(int i=0; i<ui->gridLayout->count(); i++)
    {
        VMWidget *vm = qobject_cast<VMWidget*>(ui->gridLayout->itemAt(i)->widget());
        vm->GetLabel()->setText(vmArray[i].name);
    }
}

void HomeWindow::on_minPtn_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}


void HomeWindow::on_maxPtn_clicked()
{
    if(this->windowState() == Qt::WindowNoState)
    {
        this->setWindowState(Qt::WindowMaximized);
        ui->maxPtn->setIcon(QIcon(":/image/maximum.png"));
    }
    else if(this->windowState() == Qt::WindowMaximized)
    {
        this->setWindowState(Qt::WindowNoState);
        ui->maxPtn->setIcon(QIcon(":/image/maximum.png"));
    }
}
