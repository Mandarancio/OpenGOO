#include "mainwidget.h"
#include <QDebug>



MainWidget::MainWidget(QRect geometry,bool debug,QWidget *parent)
    : QMainWindow(parent)
{
    this->showFullScreen();//To have the game full screen
    this->setGeometry(geometry);
    if (!debug)
        level=new Level(geometry,"l01.level",STANDARD,this); //Create the level
    else
        level=new Level(geometry,"l01.level",DEBUG,this); //Create the level

    level->show(); //Show it
    connect(level,SIGNAL(closing()),this,SLOT(close())); //Connect the closing of the level with the closing of the game

}

MainWidget::~MainWidget()
{
    delete level;
}

