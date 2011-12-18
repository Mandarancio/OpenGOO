#include "mainwidget.h"
#include <QDebug>



MainWidget::MainWidget(QRect geometry,QWidget *parent)
    : QMainWindow(parent)
{
    this->showFullScreen();
    this->setGeometry(geometry);
    level=new Level(geometry,"l01.level",this);
    level->show();
    connect(level,SIGNAL(closing()),this,SLOT(close()));

}

MainWidget::~MainWidget()
{

}

