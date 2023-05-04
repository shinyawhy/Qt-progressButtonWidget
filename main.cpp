#include "progressbuttonwidget.h"

#include <QApplication>
#include <QHBoxLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget w;
    w.setPalette(QPalette(QPalette::Base, QColor("#128bf1")));
    auto btn = new progressButtonWidget;
    auto hb = new QHBoxLayout(&w);
    hb->setContentsMargins(50, 50, 50, 50);
    hb->addWidget(btn);
    w.resize(800, 800);
    //    progressButtonWidget w;
    w.show();
    return a.exec();
}
