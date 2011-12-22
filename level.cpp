#include "level.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRadialGradient>

#include "tools.h"
#include "fixedgoo.h"
#include "dynamicgoo.h"
#include "thorn.h"

#include "collisionlistener.h"

Level::Level(QRect geometry, QString level, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer|QGL::SampleBuffers),parent)
{
    this->setGeometry(geometry);

    this->grabKeyboard();
    this->grabMouse();
    this->setMouseTracking(true);

    center=geometry.center();
    translation=QPoint(0,0);

    world = new b2World(b2Vec2(0,500));

    CollisionListener *cl=new CollisionListener(this);
    world->SetContactListener(cl);

    pathLevel=level;
    readLevel(pathLevel);

    createBalls();
    createThorns();

    connect(target,SIGNAL(gooCatched(Goo*)),this,SLOT(gooCatched(Goo*)));
    connect(target,SIGNAL(towerCatch()),this,SLOT(towerCatched()));
    connect(target,SIGNAL(towerLost()),this,SLOT(towerLost()));

    step=1.0/60.0;
    drag = false;
    dragged=NULL;

    points=0;
    catched=false;

    menu=new Menu(geometry,this);
    onMenu=false;
    connect(menu,SIGNAL(eventClose()),this,SLOT(closeAll()));
    connect(menu,SIGNAL(eventResume()),this,SLOT(resume()));
    connect(menu,SIGNAL(eventRestart()),this,SLOT(restart()));

    startTimer(step*1000);
}

Level::~Level(){
    for (int i=0;i<objects.length();i++)
        world->DestroyBody(objects[i]->getBody());
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
        connect(dg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
        connect(dg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
    }
}

void Level::createThorns(){
    int xi,xe,ym;
    xi=-200.0*width()/1000.0;
    xe=280.0*width()/1000.0;
    ym=280.0*width()/1000.0;
    int r=10.0*(xe-xi)/100.0;
    int h,x;
    for (int i=0;i<r;i++){
        h=50+rand()%50;
        x=xi+rand()%(xe-xi);
        Thorn *t=new Thorn(QPoint(x,ym-h),h,world,this);
        objects.push_back(t);
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
    world->ClearForces();

    for (int i=0;i<jointsToDestroy.length();i++){
        world->DestroyJoint(jointsToDestroy[i]->getJoint());
        joints.removeAt(joints.indexOf(jointsToDestroy[i]));
        delete jointsToDestroy[i];
    }
    jointsToDestroy.clear();

    for (int i=0;i<goosToDestroy.length();i++){
        world->DestroyBody(goosToDestroy[i]->getBody());
        goos.removeAt(goos.indexOf(goosToDestroy[i]));
        delete goosToDestroy[i];
    }
    goosToDestroy.clear();

    target->checkTower(goos);
    target->applyForces(goos);
    repaint();
}

void Level::paintEvent(QPaintEvent *e){

    QPainter p(this);
    //BG Color
    p.setPen(Qt::transparent);
    p.setBrush(Qt::darkGray);


    p.setRenderHint(QPainter::Antialiasing);


    p.save();
    p.translate(center+translation);
    paintBg(p);

    if (ground) ground->paint(p);
    if (target) target->paint(p);
    if (drag && possibility.length()>1)
    {
        for (int i=0;i<possibility.length();i++)
            p.drawLine(dragged->getPPosition(),possibility[i]);
    }
    for (int i=0;i<objects.length();i++)
        objects[i]->paint(p);
    for (int i=0;i<joints.length();i++)
        if (joints[i]) joints[i]->paint(p);
    for (int i=0;i<goos.length();i++)
        if (goos[i]) goos[i]->paint(p);
    p.restore();
    paintWin(p);
    paintScore(p);
    if (onMenu) menu->paint(p);

    if (p.end()) e->accept();
    else e->ignore();
}

void Level::keyReleaseEvent(QKeyEvent *e){
    switch(e->key()){
    case (Qt::Key_Escape):
        onMenu=!onMenu;
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
    if (onMenu){
        return;
    }
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
    if (onMenu) return;
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
    if (onMenu){
        menu->mouseRelease(e);
        return;
    }
    if (drag){
        if (createJoints(dragged->getPPosition()) || dragged->hasJoint()) dragged->drop();
        else dragged->drop(mouseSpeed);
    }
    dragged=NULL;
    drag=false;
    possibility.clear();
}

void Level::destroyJoint(Joint *joint){
    jointsToDestroy.push_back(joint);
}

void Level::giveTarget(Goo *previous){ //SISTEMARE STO CAZZO DI ALGORITMO!!! Non capisco dove minchia è il problema!
    Goo *goo=dynamic_cast<Goo*>(sender());
    if (goo){
        if (!previous){
            QPoint pos=goo->getPPosition();
            Goo * next;
            bool ok=false;
            float distance=300;
            for (int i=0;i<goos.length();i++){
                if (goos[i]!=goo&&goos[i]->hasJoint()&&goos[i]->isOnGround()&&abs(goos[i]->getPPosition().y()-pos.y())<15&&(goos[i]->getVPosition()-goo->getVPosition()).Length()<=distance){
                    next=goos[i];
                    distance=(toVec(pos)-goos[i]->getVPosition()).Length();
                    ok=true;
                }
            }
            if (ok) goo->setTarget(next);
        }
        else {
            if (!catched){
                int choise=rand()%previous->getLinks().length();
                goo->setTarget(previous->getLinks().at(choise));
            }
            else {
                Goo * target=previous->getLinks().at(0);
                b2Vec2 d=this->target->getVPosition()-target->getVPosition();
                for (int i=1;i<previous->getLinks().length();i++){
                    if ((goo->getPrevious()!=target->getLinks().at(i)) && (this->target->getVPosition()-previous->getLinks().at(i)->getVPosition()).LengthSquared()<d.LengthSquared()){
                        target=previous->getLinks().at(i);
                        d=this->target->getVPosition()-previous->getLinks().at(i)->getVPosition();
                    }
                }
                goo->setTarget(target);
            }
        }
    }
}

void Level::gooCatched(Goo *goo){
    world->DestroyBody(goo->getBody());
    goos.removeOne(goo);
    points++;
}

void Level::towerCatched(){    
    catched=true;
    for (int i=0;i<goos.length();i++) goos[i]->catched();

}

void Level::towerLost(){    
    catched=false;
    for (int i=0;i<goos.length();i++) goos[i]->lost();

}

void Level::paintBg(QPainter &p){
    QColor c1,c2;
    c1.setRgb(95,141,211);
    c2.setRgb(11,23,40);
    QRadialGradient g(QPoint(0,height()/2),2*height());
    g.setColorAt(0,c1);
    g.setColorAt(1,c2);
    p.setPen(Qt::transparent);
    p.setBrush(g);
    p.drawRect(-width()/2+limit.x()-5,-height()/2-limit.y()-5,width()+limit.width()*2+5,height()-limit.height()*2);
}

void Level::paintScore(QPainter &p){
    p.setPen(Qt::white);
    QFont f;
    f.setFamily("Times");
    f.setBold(true);
    f.setPointSize(30);
    p.setFont(f);
    p.drawText(10,35,QString::number(points));
    f.setPointSize(15);
    p.setFont(f);
    p.drawText(10,55,"of "+QString::number(goal));
}

void Level::paintWin(QPainter &p){
    if (points>=goal){
        QColor bg(0,0,0,200);
        p.setBrush(bg);
        p.setPen(bg);
        p.drawRect(0,0,width(),height());
        QFont f;
        f.setFamily("Times");
        f.setBold(true);
        f.setPointSize(30);
        p.setFont(f);
        p.setPen(Qt::white);
        p.drawText(this->geometry(),Qt::AlignCenter,"Win!!");
    }
}

void Level::resume(){
    onMenu=false;
}

void Level::restart(){
    goos.clear();
    joints.clear();
    objects.clear();
    world = new b2World(b2Vec2(0,500));
    CollisionListener *cl=new CollisionListener(this);
    world->SetContactListener(cl);
    readLevel(pathLevel);
    createBalls();
    createThorns();
    points=0;
    catched=false;
    connect(target,SIGNAL(gooCatched(Goo*)),this,SLOT(gooCatched(Goo*)));
    connect(target,SIGNAL(towerCatch()),this,SLOT(towerCatched()));
    connect(target,SIGNAL(towerLost()),this,SLOT(towerLost()));
}

void Level::closeAll(){
    emit this->closing();
}

void Level::destroyGOO(){
    Goo* goo=dynamic_cast<Goo*>(sender());
    if (goo){
        goosToDestroy.push_back(goo);
    }
}

void Level::destroyJoint(Goo *a, Goo *b){
    for (int i=0;i<joints.length();i++){
        if (joints[i]->has(a,b)){
            jointsToDestroy.push_back(joints[i]);
        }
    }
}
