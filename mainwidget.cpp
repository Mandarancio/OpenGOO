#include "mainwidget.h"
#include <QDebug>

#include "svglevelloader.h"

#include "flags.h"

MainWidget::MainWidget(QRect geometry,QWidget *parent)
    : QWidget(parent)
{
    this->showFullScreen();//To have the game full screen
    this->setGeometry(geometry);
    this->geometry=geometry;

    levelS=new LevelSelector(geometry,this);//Create the level selector
    levelS->show();//Show the level selector
    level=NULL;
    connect(levelS,SIGNAL(closing()),this,SLOT(close())); //Connect the closing of the levelSelector with the closing of the game
    connect(levelS,SIGNAL(eventLevelSelected()),this,SLOT(levelSelected()));//Connect the choose of the the level to the creation of the level

}

MainWidget::~MainWidget()
{
    if (levelS!=NULL){
        delete levelS;
    }
    if (level!=NULL){
        delete level;
    }
}

void MainWidget::closeEvent(QCloseEvent *e){
    if (level!=NULL){
        level->close();
    }

    e->accept();
}


void MainWidget::levelSelected()//Create the level selected
{

    if(flag & DEBUG)qWarning()<<"Level Selected "<<levelS->getLevelSelected().toAscii();
    if(!levelS->getLevelSelected().compare("Exit")){
        this->close();
        return;
    }
    else
    {

        bgWidget =new BackGroundWidget(this);
        bgWidget->setGeometry(0,0,geometry.width(),geometry.height());
        bgWidget->show();

        level=new Level(geometry,levelS->getLevelSelected(),bgWidget,this); //Create the level
        if (flag & DEBUG) qWarning()<<"LEVEL OBJECT INITILIZED";


        delete levelS;
        levelS=NULL;

        connect(level,SIGNAL(closing()),this,SLOT(close()));//Connect the closing of the level with the closing of the game
        connect(level,SIGNAL(eventBackToMainMenu()),this,SLOT(backToMainMenu()));
        if (level->startLevel()){
            level->show();//Show the Level

            if (flag & DEBUG) qWarning()<<"level showed an signal connected.";
        }

    }
}

void MainWidget::backToMainMenu()
{
    this->show();
    if (flag & DEBUG) qWarning()<<"Back to Main Menu!";
    delete level;
    delete bgWidget;
    levelS=new LevelSelector(geometry,this);//Create the level selector
    levelS->show();//Show the level selector
    level=NULL;
    connect(levelS,SIGNAL(closing()),this,SLOT(close())); //Connect the closing of the levelSelector with the closing of the game
    connect(levelS,SIGNAL(eventLevelSelected()),this,SLOT(levelSelected()));//Connect the choose of the the level to the creation of the level delete level;
}
