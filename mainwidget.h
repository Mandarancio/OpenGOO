#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QMainWindow>
#include "level.h"

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    MainWidget(QRect geometry,QWidget *parent = 0);
    ~MainWidget();
private:
    Level *level;
};

#endif // MAINWIDGET_H
