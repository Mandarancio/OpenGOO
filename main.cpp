#include <QtGui/QApplication>
#include "mainwidget.h"

#include <QRect>
#include <QDesktopWidget>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWidget w(a.desktop()->screenGeometry());
    w.show();

    return a.exec();
}
