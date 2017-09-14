#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QWidget>
#include <QDateTime>
#include "configurefile.h"

namespace Ui {
class HomeWindow;
}

class QButtonGroup;
class QTranslator;
class QSettings;
class QMenu;

struct VM_CONFIG{
    QString name;
    QString ip;
    int port;
    bool isActive;
};

class HomeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent = 0);
    ~HomeWindow();
    void AutoStartVdiClient();
    void initLanguage();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent * event);
    void keyPressEvent(QKeyEvent *e);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void button_pressed_slot(int index);
    void lang_changed_slot(QString lang);

    void on_closePtn_clicked();
    void on_parseInstances();

    void on_minPtn_clicked();

    void on_maxPtn_clicked();

private:
    void setAppStyleSheet(const QString lang);
    void initReadOnlyFS();
    void refreshVM();
    void moveToCenter();
private:
    Ui::HomeWindow *ui;

private:
    bool m_okClose;
    bool m_Drag;
    QPoint m_DragPosition;

    QButtonGroup *m_button_group;
    QTranslator *m_translator;
    QSettings *m_settingfile;

    ConfigureFile *configFile;

    QString m_curLang;

    QVector<VM_CONFIG> vmArray;
    QString serverIp;
};


#endif // HOMEWINDOW_H
