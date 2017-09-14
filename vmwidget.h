#ifndef VMWIDGET_H
#define VMWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

namespace Ui {
class VMWidget;
}

class VMWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VMWidget(QWidget *parent = 0);
    ~VMWidget();
    QPushButton *GetButton() const;
    QLabel *GetLabel() const;
private:
    Ui::VMWidget *ui;
};

#endif // VMWIDGET_H
