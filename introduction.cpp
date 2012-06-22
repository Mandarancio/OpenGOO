#include "introduction.h"
#include "flags.h"

#include <qmath.h>
#include <QDebug>
#include <QPainter>
#include <QTimerEvent>
#include <QResizeEvent>
#include <QKeyEvent>

#include "dynamicgoo.h"

#define RADIUS 18
#define INTERVALL 4

Introduction::Introduction(QWidget *parent) :
    QWidget(parent),time(0),scale(1.0), translation(QPoint(0,0)),step(1.0/40.0)
{
    if (flag & ONLYTEXT || flag & DEBUG){
        qWarning()<<"Loading intro";
    }

    this->grabKeyboard();

    //create world
    //b2vec2(0,2000) is the gravity force

    world = new b2World(b2Vec2(0,20.0));
    world->SetAutoClearForces(true);
    world->SetContinuousPhysics(true);
    world->SetAllowSleeping(true);
    if (flag & DEBUG) qWarning()<<"World object created!";

    //LOADER initialize
    this->loader=new SvgLevelLoader("resurces/intro.level",this);
    connect(loader,SIGNAL(levelGOO(QPoint,int,int)),this,SLOT(setGoo(QPoint,int,int)));
    connect(loader,SIGNAL(levelStartArea(int,QRect,int)),this,SLOT(setStartArea(int,QRect,int)));
    connect(loader,SIGNAL(levelGround(QPoint,QList<QPoint>)),this,SLOT(setGround(QPoint,QList<QPoint>)));
    loader->parse();
    startTimer(qRound(step*1000.0));
}

Introduction::~Introduction(){
    delete loader;
    for (int i=0;i<goos.length();i++){
        delete goos[i];
    }
    goos.clear();
    for (int i=0;i<ground.length();i++){
        delete ground[i];
    }
    ground.clear();
    delete world;
}

void Introduction::timerEvent(QTimerEvent *e){
    time+=step;
    for (int i=0;i<2;i++){
        world->Step((step+step/5.0),10.0,10.0);
        world->ClearForces();
    }
    repaint(QRegion(0,0,width(),height()));
    if (time>8.0) {
        this->killTimer(e->timerId());
        emit introEnd();

    }
}
void Introduction::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing,true);
    p.setBrush(Qt::gray);
    p.setPen(Qt::transparent);
    p.drawRect(0,0,width(),height());
    p.save();
    p.scale(scale,scale);

    for (int i=0;i<ground.length();i++){
        ground[i]->paint(p);
    }
    for (int i=0;i<goos.length();i++){
        goos[i]->update();
        goos[i]->paint(p);
    }
    p.restore();
    if (time>7.0){
        QColor black(0,0,0,qRound(255.0*(time-7.0)));
        p.setBrush(black);
        p.drawRect(0,0,width(),height());
    }
}

void Introduction::resizeEvent(QResizeEvent *e){
    float rW,rH;
    rW=e->size().width()/1280.0;
    rH=e->size().height()/1024.0;
    scale=(rW<rH? rW : rH);
}

void Introduction::keyPressEvent(QKeyEvent *e){
    if (e->key()==Qt::Key_Escape){
        emit introEnd();
    }
}

void Introduction::setGround(QPoint gCenter, QList<QPoint> gList){
    Ground * g=new Ground(world,gCenter,gList,this);
    ground.push_back(g);
    if (flag & DEBUG) qWarning()<<"Ground created.";
}


void Introduction::setStartArea(int n, QRect area,int type){
    int x,y;
    x=area.x();
    y=area.y();
    int radius;
    for (int i=0;i<n;i++){
        x=area.x()+qrand()%area.width();
        y=area.y()+qrand()%area.height();
        radius=RADIUS+(qrand()%(INTERVALL*2)-INTERVALL);

        if (type==0) { //Create a standard gooo
            DynamicGoo* dg=new DynamicGoo(world,QPoint(x,y),radius);
            dg->setSleep(false);
            goos.push_back(dg);

        }
        else return;

    }
    if (flag & DEBUG) qWarning()<<"A start area is created.";
}

void Introduction::setGoo(QPoint center,int id, int type){
    int radius=RADIUS+(qrand()%(INTERVALL*2)-INTERVALL);
    if (type==0) { //Create a standard gooo
        DynamicGoo* dg=new DynamicGoo(world,center,radius);
        dg->setSleep(false);
        goos.push_back(dg);

    }
    else return;
}
