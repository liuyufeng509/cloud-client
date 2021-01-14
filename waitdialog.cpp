#include "waitdialog.h"
#include <QHBoxLayout>
WaitDialog::WaitDialog(QWidget*parent):
    QDialog(parent)
{
    label = new QLabel(this);
    this->setFixedSize(300,150);
    setWindowOpacity(1); //设置透明用；
    //this->setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    this->setWindowTitle("请稍后");
    QHBoxLayout *layout = new QHBoxLayout(this);
//    layout->setMargin(0);
//    layout->setSpacing(0);
    layout->addWidget(label);
    label->setText("请稍后");
    //取消对话框标题
    //this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    //取消对话框标题和边框
    //this->setAutoFillBackground(true);
    this->setContentsMargins(0,0,0,0);
   // label->setContentsMargins(0,0,0,0);
    movie = new QMovie(":/new/index/loading.gif");
    movie->setScaledSize(QSize(300,150));
    label->setMovie(movie);
    movie->start();
}
WaitDialog::~WaitDialog()
{
    delete label;
    delete movie;
}
