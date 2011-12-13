#include "level.h"
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "tools.h"
#include "fixedgoo.h"
#include "dynamicgoo.h"

#include "collisionlistener.h"

Level::Level(QRect geometry, QString level, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer),parent)
{
    this->setGeometry(geometry);

    this->grabKeyboard();
    this->grabMouse();
    this->setMouseTracking(true);

    center=geometry.center();
    translation=QPoint(0,0);

    world = new b2World(b2Vec2(0,200));

    CollisionListener *cl=new CollisionListener(this);
    world->SetContactListener(cl);

    readLevel(level);

    createBalls();


    step=1.0/60.0;
    drag = false;
    dragged=NULL;
    startTimer(step*1000);
}

Level::~Level(){
    delete world;
}

void Level::readLevel(QString path){
    QFile file(path);
    if (file.open(QFile::ReadOnly)){
        QTextStream input(&file);
        QString tmp;
        int flags=0;
        while (!input.atEnd()){
            tmp=input.readLine();
            if (tmp[0]=='#') continue;
            if (parseString(tmp)) flags++;

        }
        if (flags!=9) {
            qWarning()<<"File malformed!";
            emit closing();
            return;
        }
        file.close();
    }
    else {
        qWarning()<<"File not found!!";

        emit this->closing();
        this->close();
    }
}

bool Level::parseString(QString string)
{
    QString tag=string.split(':').at(0);
    QString info;
    if (string.split(':').length()==2) info=string.split(':').at(1);
    else{
        qWarning()<<"File malformated";
        emit this->closing();
        return false;
    }
    if (parseInfo(tag,info)) return true;
    else return false;
}

bool Level::parseInfo(QString tag, QString info){
    int wC,hC;
    wC=this->width();
    hC=this->height();
    if (!tag.compare("name",Qt::CaseInsensitive)){
        name=info;
        return true;
    }
    if (!tag.compare("goal",Qt::CaseInsensitive)){
        bool ok;
        goal=info.toInt(&ok);
        return ok;
    }
    if (!tag.compare("nBalls",Qt::CaseInsensitive)){
        bool ok;
        nBalls=info.toInt(&ok);
        return ok;
    }
    if (!tag.compare("start area",Qt::CaseInsensitive)){
        if (info.split(',').length()==2){
            bool ok=true;
            startArea=pRect(info,ok,wC,hC);
            return ok;
        }
        else return false;
    }
    if (!tag.compare("limit",Qt::CaseInsensitive)){
        if (info.split(',').length()==2){
            bool ok=true;
            limit=QRect(pList(info,ok,wC,hC,2).at(0),pList(info,ok,wC,hC,2).at(1));
            return ok;
        }
        else return false;
    }
    if (!tag.compare("start joint",Qt::CaseInsensitive)){
        if (info.split(',').length()==2){
            bool ok=true;
            QPoint a,b;
            a=pList(info,ok,wC,hC,2).at(0);
            b=pList(info,ok,wC,hC,2).at(1);
            if (!ok) return false;
            a.setX(a.x()+15);
            a.setY(a.y()-15);
            b.setX(b.x()+15);
            b.setY(b.y()-15);
            FixedGoo* aG,*bG;
            aG=new FixedGoo(world,a);
            bG=new FixedGoo(world,b);
            goos.push_back(aG);
            goos.push_back(bG);

            return makeJoint(aG,bG);

        }
        else return false;
    }
    if (!tag.compare("start force",Qt::CaseInsensitive)){
        bool ok;
        startForce=toVec(pPoint(info,ok));
        return ok;
    }
    if (!tag.compare("target",Qt::CaseInsensitive)){
        bool ok;
        QPoint tPoint=pPoint(info,ok,wC,hC);
        if (!ok) return false;
        target=new Target(tPoint,hC,world,this);
        return true;
    }
    if (!tag.compare("ground",Qt::CaseInsensitive)){
        if (info.split('|').length()!=2) return false;
        QString center,list;
        center=info.split('|').at(0);
        list=info.split('|').at(1);
        bool ok=true;
        QPoint gCenter=pPoint(center,ok,wC,hC);

        if (!ok) return false;
        QList <QPoint> gList=pList(list,ok,wC,hC,2);
        if (!ok) return false;
        ground= new Ground(world,gCenter,gList,this);
        return true;
    }
    return false;
}

void Level::createBalls(){
    int x,y;
    for (int i=0;i<nBalls;i++){
        x=startArea.x()+rand()%startArea.width();
        y=startArea.y()+rand()%startArea.height();
        DynamicGoo* dg=new DynamicGoo(world,QPoint(x,y));
        dg->getBody()->ApplyForceToCenter(startForce);
        goos.push_back(dg);
        connect(dg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
    }
}

Goo* Level::getGooAt(QPoint p){
    b2Vec2 d;
    for (int i=0;i<goos.count();i++){
        if (goos[i]->isMoovable()&&goos[i]->isDragable()){
            d=toVec(p)-goos[i]->getVPosition();
            if (d.Length()<goos[i]->getRadius()) return goos[i];
        }
    }
    return NULL;
}

void Level::moveUp(){
    if (translation.y()<limit.y())
        translation.setY(translation.y()+5);
}
void Level::moveDown(){
    if (translation.y()>(limit.height()))
        translation.setY(translation.y()-5);
}
void Level::moveRight(){
    if (translation.x()>-(limit.width()-abs(limit.x())))
        translation.setX(translation.x()-5);
}
void Level::moveLeft(){
    if (translation.x()<-limit.x())
        translation.setX(translation.x()+5);
}

bool Level::makeJoint(Goo *a, Goo *b){
    if (!a->createLink(b)) return false;
    if (!b->createLink(a)) {
        a->destroyLink(b);
        return false;
    }
    Joint* j=new Joint(a,b,world,this);
    joints.push_back(j);
    connect(j,SIGNAL(destroyJoint(Joint*)),this,SLOT(destroyJoint(Joint*)));
    return true;
}

QList<QPoint> Level::possibleJoints(QPoint p){
    QList<QPoint> l;
    b2Vec2 pv=toVec(p);
    b2Vec2 d;
    for (int i=0;i<goos.length();i++){
        if (goos[i]->canHaveJoint()) {
            d=pv-goos[i]->getVPosition();
            if (d.LengthSquared()>=50*50 && d.LengthSquared()<=150*150 )
                l.push_back(goos[i]->getPPosition());

        }
    }
    return l;
}

bool Level::createJoints(QPoint p){
    QList<Goo*> l;
    b2Vec2 pv=toVec(p);
    b2Vec2 d;
    for (int i=0;i<goos.length();i++){
        if (goos[i]->canHaveJoint()) {
            d=pv-goos[i]->getVPosition();
            if (d.LengthSquared()>=50*50 && d.LengthSquared()<=150*150 && !l.contains(goos[i]))
                l.push_back(goos[i]);

        }
    }
    if (l.length()>1||dragged->hasJoint()){
        for (int i=0;i<l.length();i++){
            if (makeJoint(dragged,l[i])) continue;
        }
        return true;
    }
    else return false;
}

void Level::timerEvent(QTimerEvent *e){
    e->accept();
    world->Step(step,10,10);
    repaint();
}

void Level::paintEvent(QPaintEvent *e){

    QPainter p(this);
    //BG Color
    p.setPen(Qt::transparent);
    p.setBrush(Qt::darkGray);

    p.drawRect(this->geometry());

    p.setRenderHint(QPainter::Antialiasing);
    p.save();
    p.translate(center+translation);

    if (ground) ground->paint(p);
    if (target) target->paint(p);
    if (drag && possibility.length())
    {
        for (int i=0;i<possibility.length();i++)
            p.drawLine(dragged->getPPosition(),possibility[i]);
    }
    for (int i=0;i<joints.length();i++)
        if (joints[i]) joints[i]->paint(p);
    for (int i=0;i<goos.length();i++)
        if (goos[i]) goos[i]->paint(p);
    p.restore();
    if (p.end()) e->accept();
    else e->ignore();
}

void Level::keyReleaseEvent(QKeyEvent *e){
    switch(e->key()){
    case (Qt::Key_Escape):
        this->closing();
        break;
    case (Qt::Key_Up):
        moveUp();
        break;
    case (Qt::Key_Down):
        moveDown();
        break;
    case (Qt::Key_Left):
        moveLeft();
        break;
    case (Qt::Key_Right):
        moveRight();
        break;
    }
}


void Level::mouseMoveEvent(QMouseEvent *e){
    if (e->x()<=5) moveLeft();
    if (e->y()<=5) moveUp();
    if (e->x()>=width()-5) moveRight();
    if (e->y()>=height()-5) moveDown();
    if (drag){
        mouseSpeed=(toVec(e->pos())-mousePos);
        mouseSpeed.x*=1000;
        mouseSpeed.y*=1000;
        mousePos=toVec(e->pos());
        dragged->move(e->pos()-(center+translation));
        possibility=possibleJoints(dragged->getPPosition());
    }
}
void Level::mousePressEvent(QMouseEvent *e){
   if (e->button()==Qt::LeftButton ) {
       dragged=getGooAt(e->pos()-(center+translation));
       if (dragged) {
           possibility.clear();
           drag=true;
           dragged->drag();
           mousePos=toVec(e->pos());
           mouseSpeed.SetZero();
       }
   }
}
void Level::mouseReleaseEvent(QMouseEvent *e){
    if (drag){
        if (createJoints(dragged->getPPosition()) || dragged->hasJoint()) dragged->drop();
        else dragged->drop(mouseSpeed);
    }
    dragged=NULL;
    drag=false;
    possibility.clear();
}

void Level::destroyJoint(Joint *joint){
    world->DestroyJoint(joint->getJoint());
    joints.removeOne(joint);
    delete joint;
}

void Level::giveTarget(Goo *previous){
    Goo *goo=dynamic_cast<Goo*>(sender());
    if (goo){
        if (!previous){
            QPoint pos=goo->getPPosition();
            Goo * next;
            bool ok=false;
            float distance=300;
            for (int i=0;i<goos.length();i++){
                if (goos[i]!=goo&&goos[i]->hasJoint()&&abs(goos[i]->getPPosition().y()-pos.y())<15&&(goos[i]->getVPosition()-goo->getVPosition()).Length()<=distance){
                    next=goos[i];
                    distance=(toVec(pos)-goos[i]->getVPosition()).Length();
                    ok=true;
                }
            }
            if (ok) goo->setTarget(next);
        }
        else {
            int choise=rand()%previous->getLinks().length();
           goo->setTarget(previous->getLinks().at(choise));
        }
    }
}
