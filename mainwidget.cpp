#include "mainwidget.h"
#include <QDebug>



MainWidget::MainWidget(QRect geometry,QWidget *parent)
    : QMainWindow(parent)
{
    this->showFullScreen();//To have the game full screen
    this->setGeometry(geometry);
    level=new Level(geometry,"l01.level",this); //Create the level
    level->show(); //Show it
    connect(level,SIGNAL(closing()),this,SLOT(close())); //Connect the closing of the level with the closing of the game

}

MainWidget::~MainWidget()
{
    delete level;
}

