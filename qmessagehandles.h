#ifndef QMESSAGEHANDLES_H
#define QMESSAGEHANDLES_H

#include <QObject>

class QMessageHandles : public QObject
{
    Q_OBJECT
public:

    static QMessageHandles* instance();

    void ReceiveMassage(const uint dwContext, const uint evtID, const QString param);
private:
    explicit QMessageHandles(QObject *parent = nullptr);
    void showTip(QStringList sl, int timeout);

    static QMessageHandles* s_inst;

signals:
    void loginReady(bool success, QString erroInfo);
    void DeskTipMessage(const QStringList &texts, int timeout);   //屏幕右下角显示框的内容
    void servicesList(QString services);
    void UpdateServciesList(QString services);
public slots:
};

#endif // QMESSAGEHANDLES_H
