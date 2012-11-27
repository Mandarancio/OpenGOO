#include <QDebug>
#include <logger.h>

#include "svglevelloader.h"
#include "introduction.h"
#include "mainwidget.h"
#include "flags.h"

#include "mainmenu.h"

MainWidget::MainWidget(QRect geometry,QWidget *parent)
    : QWidget(parent)
{
    this->setGeometry(geometry);
    this->releaseKeyboard();

    intro=new Introduction(this);
    intro->show();

    layout=new QGridLayout(this);
    layout->setMargin(0);
    layout->addWidget(intro,0,0,0,0);

    level=NULL;
    levelS=NULL;

    connect(intro,SIGNAL(introEnd()),this,SLOT(startSelection()));
}

MainWidget::~MainWidget()
{
    delete levelS;
    delete level;
    SoundSystem::DestroyInstance();
}

void MainWidget::closeEvent(QCloseEvent *e){

    if (level!=NULL){
        level->close();
    }
  e->accept();
}


void MainWidget::levelSelected() //Create the level selected
{

    if(flag & DEBUG)logWarn(QString("Level Selected %1").arg(levelS->getLevelSelected()));
    if(!levelS->getLevelSelected().compare("Exit")){
        this->close();
        return;
    }
    else
    {
        QString levelselected=levelS->getLevelSelected();
        delete levelS;
        levelS=NULL;

        layout->removeWidget(levelS);
        bgWidget =new BackGroundWidget(this);
        bgWidget->setGeometry(0,0,width(),height());
        bgWidget->show();
        layout->addWidget(bgWidget,0,0,0,0);
        this->setLayout(layout);

        level=new Level(levelselected,bgWidget,this); //Create the level

        layout->addWidget(level,0,0,0,0);

        if (flag & DEBUG) logWarn("LEVEL OBJECT INITIALIZED.");

        connect(level,SIGNAL(closing()),this,SLOT(close()));//Connect the closing of the level with the closing of the game
        connect(level,SIGNAL(eventBackToMainMenu()),this,SLOT(backToMainMenu()));
        if (level->startLevel()){
            level->show();//Show the Level

            if (flag & DEBUG) logWarn("level showed an signal connected.");
        }

    }
}

void MainWidget::backToMainMenu()
{
    this->show();
    if (flag & DEBUG) logWarn("Back to Main Menu!");
    layout->removeWidget(level);
    layout->removeWidget(bgWidget);
    delete level;
    delete bgWidget;
    levelS=new LevelSelector(this);                                             //Create the level selector
    levelS->show();                                                             //Show the level selector
    layout->addWidget(levelS,0,0,0,0);
    level=NULL;
    connect(levelS,SIGNAL(closing()),this,SLOT(close()));                       //Connect the closing of the levelSelector with the closing of the game
    connect(levelS,SIGNAL(eventLevelSelected()),this,SLOT(levelSelected()));    //Connect the choose of the the level to the creation of the level delete level;
}

void MainWidget::startSelection()
{
    layout->removeWidget(intro);
    delete intro;
    levelS=new LevelSelector(this);                                             //Creates the level selector
    levelS->show();                                                             //Shows the level selector
    connect(levelS,SIGNAL(closing()),this,SLOT(close()));                       //Connect the closing of the levelSelector with the closing of the game
    connect(levelS,SIGNAL(eventLevelSelected()),this,SLOT(levelSelected()));    //Connect the choose of the the level to the creation of the level
    layout->addWidget(levelS,0,0,0,0);
}

