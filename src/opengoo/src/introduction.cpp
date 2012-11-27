#include "introduction.h"
#include "flags.h"

#include <qmath.h>
#include <QDebug>
#include <logger.h>
#include <QPainter>
#include <QTimerEvent>
#include <QResizeEvent>
#include <QKeyEvent>

#include "collisionlistener.h"
#include "dynamicgoo.h"
#include "soundsystem.h"
#include "publicclass.h"

#define RADIUS 18
#define INTERVALL 4

Introduction::Introduction(QWidget *parent) :
    QWidget(parent),time(0),scale(1.0),step(1.0/40.0)
{
    sSystem = SoundSystem::GetInstance();
    sSystem->Open();
    if (flag & ONLYTEXT || flag & DEBUG){
        logWarn("Loading intro");
    }

    this->grabKeyboard();

    //create world
    //b2vec2(0,2000) is the gravity force

    world = new b2World(b2Vec2(0,15.0));
    world->SetAutoClearForces(true);
    world->SetContinuousPhysics(true);
    world->SetAllowSleeping(true);
    world->SetContactListener(new CollisionListener());

    if (flag & DEBUG) logWarn("World object created!");

    //LOADER initialization:

    this->loader=new SvgLevelLoader("resources/intro.level",this);

    connect(loader,SIGNAL(levelGOO(QPoint,int,int)),this,SLOT(setGoo(QPoint,int,int)));
    connect(loader,SIGNAL(levelStartArea(int,QRect,int)),this,SLOT(setStartArea(int,QRect,int)));
    connect(loader,SIGNAL(levelGround(QPoint,QList<QPoint>)),this,SLOT(setGround(QPoint,QList<QPoint>)));
    loader->parse();

    itime.start();
    startTimer(qRound(step*1000.0));
    time=itime.elapsed();
    logWarn(QString("Time elapsed %1").arg(time));
    nFrame=0;
    endTime=7.0;
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
    sSystem->Close();
}

void Introduction::timerEvent(QTimerEvent *e){
    nFrame++;
    float old=time;
    time=itime.elapsed()/1000.0;
    realStep= (time-old);

    int n=qRound(6.0*realStep/step);

    for (int i=0;i<n;i++){
        world->Step(0.01,10.0,10.0);
        world->ClearForces();
    }
    repaint(QRegion(0,0,width(),height()));

    if (time>endTime+1.0) {
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
    if (time<1.0){
        QColor bg(0,0,0,qRound(time*180.0));
        QColor fg(255,255,255,qRound(time*200.0));
        QFont f("mono",14);
        f.setBold(true);
        p.setFont(f);
        p.setBrush(bg);
        p.setPen(Qt::transparent);
        p.drawRoundedRect(15,height()-55,248,40,10,10);
        p.setPen(fg);
        p.drawText(25,height()-28,"Press Esc to skip...");
    }
    else {
        p.setBrush(QColor(0,0,0,180));
        p.setPen(Qt::transparent);
        p.drawRoundedRect(15,height()-55,248,40,10,10);
        p.setPen(QColor(255,255,255,200));
        QFont f("mono",14);
        f.setBold(true);
        p.setFont(f);
        p.drawText(25,height()-28,"Press Esc to skip...");

    }
    if (time>endTime){
        QColor black(0,0,0,qRound(255.0*(time-endTime)));
        p.setBrush(black);
        p.drawRect(0,0,width(),height());
    }
}

void Introduction::resizeEvent(QResizeEvent *e){
    float rW,rH;
    rW=e->size().width()/1280.0;
    rH=e->size().height()/1024.0;
    scale=(rW<rH? rW : rH);
    sSystem->SetCenter(QPoint(e->size().width()*scale/2,e->size().height()*scale/2));
}

void Introduction::keyPressEvent(QKeyEvent *e){
    if (e->key()==Qt::Key_Escape && time<7.0 && endTime==7.0){
        endTime=time;
    }
}

void Introduction::setGround(QPoint gCenter, QList<QPoint> gList){
    Ground * g=new Ground(world,gCenter,gList,this);
    ground.push_back(g);
    if (flag & DEBUG) logWarn("Ground created.");
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
    if (flag & DEBUG) logWarn("A start area is created.");
}

void Introduction::setGoo(QPoint center,int id, int type){
    Q_UNUSED(id)
    int radius=RADIUS+(qrand()%(INTERVALL*2)-INTERVALL);
    if (type==0) { //Create a standard gooo
        DynamicGoo* dg=new DynamicGoo(world,center,radius);
        dg->setSleep(false);
        goos.push_back(dg);

    }
    else return;
}

