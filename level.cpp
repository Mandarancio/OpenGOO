#include "level.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRadialGradient>

#include "tools.h"
#include "fixedgoo.h"
#include "dynamicgoo.h"
#include "removablegoo.h"
#include "thorn.h"
#include "stickylink.h"

#include "collisionlistener.h"

#include "levelloader.h"

#include <QPolygon>

#define RADIUS 15

Level::Level(QRect geometry, QString level,RunFlag flag, QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers),parent)
{

    //Set enviroment flag
    this->flag=flag;

    //set the display geometry
    this->setGeometry(0,0,geometry.width(),geometry.height());
    if (flag==DEBUG) qWarning()<<"Geometry setted:"<<geometry;

    //grab keyboard, mouse and track it!
    this->grabKeyboard();
    this->grabMouse();
    this->setMouseTracking(true);
    if (flag==DEBUG) qWarning()<<"Mouse and keyboard grabbed";

    //compute the center of the display
    //Fix for multi screen
    center=(geometry.x() == 0 ? geometry.center() : geometry.center()-geometry.topLeft());
    //initialize translation values
    translation=QPoint(0,0);

    //create world
    //b2vec2(0,2000) is the gravity force
    world = new b2World(b2Vec2(0,400.0));
    if (flag==DEBUG) qWarning()<<"World object created!";

    //setup our modified collisionlistener
    CollisionListener *cl=new CollisionListener(this);
    world->SetContactListener(cl);
    if (flag==DEBUG) qWarning()<<"Collision listener created and set up!";

    //setup the leveloader with some enviroment parameters
    loader=new LevelLoader(level);
    loader->setDisplay(width(),height());
    //connect the loader signals
    connect(loader,SIGNAL(fileError()),this,SLOT(closeAll()));
    connect(loader,SIGNAL(levelName(QString)),this,SLOT(setName(QString)));
    connect(loader,SIGNAL(levelGoal(int)),this,SLOT(setGoal(int)));
    connect(loader,SIGNAL(levelGround(QPoint,QList<QPoint>)),this,SLOT(setGround(QPoint,QList<QPoint>)));
    connect(loader,SIGNAL(levelLimit(QRect)),this,SLOT(setLimit(QRect)));
    connect(loader,SIGNAL(levelTarget(QPoint)),this,SLOT(setTarget(QPoint)));
    connect(loader,SIGNAL(levelJoint(QPoint,QPoint)),this,SLOT(setJoint(QPoint,QPoint)));
    connect(loader,SIGNAL(levelStartArea(int,QRect,int)),this,SLOT(setStartArea(int,QRect,int)));
    if (flag==DEBUG) qWarning()<<"Level loader created, set up and connected!";

    //load the level
    loader->load();
    if (flag==DEBUG) qWarning()<<"Level loaded!";

    //connect target signals with level
    connect(target,SIGNAL(gooCatched(Goo*)),this,SLOT(gooCatched(Goo*)));
    connect(target,SIGNAL(towerCatch()),this,SLOT(towerCatched()));
    connect(target,SIGNAL(towerLost()),this,SLOT(towerLost()));
    if (flag==DEBUG) qWarning()<<"Target connected!";

    //setup the step variable
    //this one is the interval between step
    step=1.0/60.0;
    //initialize variables for draggin goo
    drag = false;
    dragged=NULL;
    //
    //mouseJoint=NULL;

    points=0;
    catched=false;
    if (flag==DEBUG) qWarning()<<"Game variable initialized!";


    menu=new Menu(geometry,this,flag==DEBUG);
    onMenu=false;
    mooving=false;
    connect(menu,SIGNAL(eventClose()),this,SLOT(closeAll()));
    connect(menu,SIGNAL(eventResume()),this,SLOT(resume()));
    connect(menu,SIGNAL(eventRestart()),this,SLOT(restart()));
    connect(menu,SIGNAL(eventBackToMainMenu()),this,SLOT(backToMainMenu()));
    if (flag==DEBUG) qWarning()<<"Menu set up!";

    startTimer(step*1000);
    if (flag==DEBUG) qWarning()<<"Timer started!"<<"Time step is:"<<step<<"second";
}

Level::~Level(){
    //clear object bodies
    for (int i=0;i<objects.length();i++){
        world->DestroyBody(objects[i]->getBody());
        delete objects[i];
    }
    objects.clear();
    //clear joints
    for (int i=0;i<joints.length();i++){
        world->DestroyJoint(joints[i]->getJoint());
        delete joints[i];
    }
    joints.clear();
    //clear stickies;
    for (int i=0;i<stickys.length();i++){
        world->DestroyJoint(stickys[i]->getJoint());
        delete stickys[i];
    }
    stickys.clear();
    //clear goo body
    for (int i=0;i<goos.length();i++){
        world->DestroyBody(goos[i]->getBody());
        delete goos[i];
    }
    goos.clear();
    //clear ground.
    world->DestroyBody(ground->getBody());
    delete ground;
    //clear world.
    delete world;
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

void Level::moveOf(QPoint dP){
    int xf,yf;
    xf=translation.x()+dP.x()/2;
    yf=translation.y()+dP.y()/2;
    if (xf>= -limit.x()) xf=-limit.x();
    else if (xf<=-(limit.width()-abs(limit.x()))) xf= -(limit.width()-abs(limit.x()));
    if (yf>=limit.y()) yf=limit.y();
    else if (yf<=limit.height()) yf=limit.height();
    translation=QPoint(xf,yf);
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
    for (int i=0;i<stickys.length();i++) stickys[i]->checkStatus();
    for (int i=0;i<stickyToCreate.length();i++){
        QPair<Goo*,QPoint> p= stickyToCreate.at(i);
        StickyLink*sl=new StickyLink(p.first,ground->getBody(),p.second,world,4);
        stickys.push_back(sl);
        connect(sl,SIGNAL(destroySticky()),this,SLOT(destroySticky()));
    }
    for (int i=0;i<jointsToDestroy.length();i++){
        if (!joints.contains(jointsToDestroy[i]))  continue;
        else {
            world->DestroyJoint(jointsToDestroy[i]->getJoint());
            joints.removeAt(joints.indexOf(jointsToDestroy[i]));
            delete jointsToDestroy[i];
        }
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
    stickyToCreate.clear();
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
    for (int i=0;i<joints.length();i++) {
        if (joints[i]) {
            joints[i]->paint(p);
            if (flag==DEBUG){
                joints[i]->paintDebug(p);
            }
        }
    }
    for (int i=0;i<goos.length();i++){
        if (goos[i]) {
            goos[i]->paint(p);
            if (flag==DEBUG){
                goos[i]->paintDebug(p);
            }
        }
    }
    if (flag==DEBUG) {
        for (int i=0;i<stickys.length();i++)
            stickys[i]->paint(p);
    }


    p.restore();

    p.save();
    p.translate(center);

    if (flag==DEBUG){
        p.setPen(Qt::white);
        p.drawLine(0,5,0,-5);
        p.drawLine(5,0,-5,0);
    }
    p.restore();

    paintTargetArrow(p);
    paintWin(p);
    paintScore(p);
    if (onMenu) menu->paint(p);
    paintButton(p);
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
    if (drag && e->x()<=5) moveLeft();
    if (drag && e->y()<=5) moveUp();
    if (drag && e->x()>=width()-5) moveRight();
    if (drag && e->y()>=height()-5) moveDown();
    if (drag){
        mouseSpeed=(toVec(e->pos())-mousePos);
        mouseSpeed.x*=10000;
        mouseSpeed.y*=10000;
        mousePos=toVec(e->pos());
        dragged->move(e->pos()-(center+translation));
        possibility=possibleJoints(dragged->getPPosition());
        //For mouse joint implementation
//        if (mouseJoint!=NULL)
//            mouseJoint->SetTarget(toVec(e->pos()-(center+translation)));
    }
    else if (mooving) {
        QPoint d=e->pos()-toPoint(mousePos);
        mousePos=toVec(e->pos());
        moveOf(d);
    }
}
void Level::mousePressEvent(QMouseEvent *e){
    if (onMenu || points>=goal) return;
    if (e->button()==Qt::LeftButton ) {
        mousePos=toVec(e->pos());
        mouseSpeed.SetZero();
       dragged=getGooAt(e->pos()-(center+translation));
       if (dragged) {
           possibility.clear();
           drag=true;
           dragged->drag();

           //Mouse joint implementation
//           b2MouseJointDef def;
//           def.bodyA=ground->getBody();
//           def.bodyB=dragged->getBody();
//           def.target=toVec(e->pos()-(center+translation));
//           def.maxForce=1000000;
//           def.frequencyHz=1/step;
//           def.dampingRatio=1000;
//           mouseJoint=(b2MouseJoint*)world->CreateJoint(&def);

       }
       else mooving=true;
   }
}
void Level::mouseReleaseEvent(QMouseEvent *e){
    if (e->button()!=Qt::LeftButton) return;
    if (!drag){
        clickButton(e->pos());
    }
    else if (drag){
        if (createJoints(dragged->getPPosition()) || dragged->hasJoint()) dragged->drop();
        else dragged->drop(mouseSpeed);
        //For mousejoint implementation
//        if (mouseJoint!=NULL){
//            world->DestroyJoint(mouseJoint);
//            mouseJoint=NULL;
//        }

    }
    dragged=NULL;
    drag=false;
    possibility.clear();
    mooving=false;


    if (onMenu){
        menu->mouseRelease(e);
        return;
    }
}

void Level::destroyJoint(Joint *joint){
    jointsToDestroy.push_back(joint);
}

void Level::giveTarget(Goo *previous){ //SISTEMARE STO CAZZO DI ALGORITMO!!! Non capisco dove minchia è il problema!
    Goo *goo=dynamic_cast<Goo*>(sender());
    if (goo!=NULL){
        if (!previous){
            QPoint pos=goo->getPPosition();
            Goo * next=NULL ;
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
            if (!catched && previous!=NULL && previous->getLinks().length()){
                int choise=rand()%previous->getLinks().length();
                goo->setTarget(previous->getLinks().at(choise));
            }
            else if (previous->getLinks().length()) {
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
    p.drawRect(-width()/2+limit.x()-5,-height()/2-limit.y()-5,width()+limit.width()*2+5,(height()-limit.height())*2);
}

void Level::paintScore(QPainter &p){
    p.setPen(Qt::white);
    QFont f;
    f.setFamily("Times");
    f.setBold(true);
    f.setPointSize(30);
    p.setFont(f);
    p.drawText(10,height()-26,QString::number(points));
    f.setPointSize(15);
    p.setFont(f);
    p.drawText(10,height()-7,"of "+QString::number(goal));
}

//Function to paint the win screen
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

//Function to paint the target arrow
void Level::paintTargetArrow(QPainter &p){
    //Check if the target is displayed:
    QPoint tp= toPoint(target->getVPosition());
    QRect darea(translation,this->geometry().size());
    if (!darea.contains(center-tp)){
        //Target is not displayed.
        tp=translation+tp;
        //Translate at the center of the area and rotate in the direction of the target
        p.save();
        p.setBrush(Qt::black);
        p.translate(center);
        p.setPen(Qt::black);
        //Compute the angle of the vector between center and target
        float angle=atan2(-tp.y(),-tp.x())*180.0/3.141628-180;
        //Compute the distance to borders:
        float m1,m2;
        m1=abs(width()/(2*cos(angle*3.141628/180.0)));
        m2=abs(height()/(2*sin(angle*3.141628/180.0)));
        int d=(m1<m2? m1 : m2)-60;
        p.rotate(angle);
        p.translate(d,0);
        //create the arrow polygon
        QPolygon arrow(3);
        arrow.insert(0,QPoint(0,15));
        arrow.insert(1,QPoint(30,0));
        arrow.insert(2,QPoint(0,-15));
        //Draw it
        p.drawPolygon(arrow);
        //Restor evrything
        p.restore();
    }

}

//Function to paint the menu button (for touchscreen device)
void Level::paintButton(QPainter &p){
    p.setPen(Qt::darkGray);
    p.setBrush(QColor(255,255,255,60));
    p.drawEllipse(QPoint(this->width(),this->height()),60,60);
    p.drawEllipse(QPoint(this->width()-20,this->height()-20),7,7);
}

//Detect if the button menu is clicked
void Level::clickButton(QPoint p){
    QPoint d=p-QPoint(width(),height());
    if (d.x()*d.x()+d.y()*d.y()<60*60) onMenu=!onMenu;
}

//Resume at game from the menu
void Level::resume(){
    onMenu=false;
}

void Level::restart(){
    for (int i=0;i<joints.length();i++){
        world->DestroyJoint(joints[i]->getJoint());
        delete joints[i];
    }
    joints.clear();

    for (int i=0;i<stickys.length();i++){
        world->DestroyJoint(stickys[i]->getJoint());
        delete stickys[i];
    }
    stickys.clear();

    for (int i=0;i<goos.length();i++){
        world->DestroyBody(goos[i]->getBody());
        delete goos[i];
    }
    goos.clear();
    for (int i=0;i<objects.length();i++){
        world->DestroyBody(objects[i]->getBody());
        delete objects[i];
    }
    objects.clear();
    delete world;

    world = new b2World(b2Vec2(0,500));
    CollisionListener *cl=new CollisionListener(this);
    world->SetContactListener(cl);
    loader->load();
    //createThorns();
    points=0;
    catched=false;
    onMenu=false;
    connect(target,SIGNAL(gooCatched(Goo*)),this,SLOT(gooCatched(Goo*)));
    connect(target,SIGNAL(towerCatch()),this,SLOT(towerCatched()));
    connect(target,SIGNAL(towerLost()),this,SLOT(towerLost()));
}

void Level::closeAll(){
    emit this->closing();
}

void Level::destroyGOO(){
    Goo* goo=dynamic_cast<Goo*>(sender());
    if (goo && !goosToDestroy.contains(goo)){
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

//LOADER FUNCTION

void Level::setName(QString name){
    this->name=name;
}

void Level::setGoal(int goal){
    if (flag==DEBUG) qWarning()<<"Goal:"<<goal;
    this->goal=goal;
}

void Level::setLimit(QRect limit){
    if (flag==DEBUG) qWarning()<<"Level limit:"<<limit;
    this->limit=limit;
}

void Level::setGround(QPoint gCenter, QList<QPoint> gList){
    ground=new Ground(world,gCenter,gList,this);
    if (flag==DEBUG) qWarning()<<"Ground created.";
}

void Level::setTarget(QPoint target){
    if (flag==DEBUG) qWarning()<<"Target at:"<<target;
    this->target=new Target(target,height(),world,this);
}

void Level::setStartArea(int n, QRect area,int type){
    int x,y;
    for (int i=0;i<n;i++){
        x=area.x()+rand()%area.width();
        y=area.y()+rand()%area.height();
        if (type==0) { //Create a standard gooo
                DynamicGoo* dg=new DynamicGoo(world,QPoint(x,y),RADIUS);
                goos.push_back(dg);
                connect(dg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
                connect(dg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
                connect(dg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
                connect(dg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
                connect(dg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
                connect(dg,SIGNAL(stopDragging()),this,SLOT(stopDragging()));
        }
        else if (type==1){ //Create a removable goo
                RemovableGoo* rg=new RemovableGoo(world,QPoint(x,y),RADIUS);
                goos.push_back(rg);
                connect(rg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
                connect(rg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
                connect(rg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
                connect(rg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
                connect(rg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
                connect(rg,SIGNAL(stopDragging()),this,SLOT(stopDragging()));
        }
        else if (type==2){ //Create a fixed goo
                FixedGoo* fg=new FixedGoo(world,QPoint(x,y),RADIUS);
                goos.push_back(fg);
                connect(fg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
                connect(fg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
                connect(fg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
        }
        else return;
    }
    if (flag==DEBUG) qWarning()<<"A start area is created.";
}

void Level::setJoint(QPoint a, QPoint b){
    QPoint ga(a.x()+RADIUS,a.y()-RADIUS-2);
    QPoint gb(b.x()+RADIUS,b.y()-RADIUS-2);
    DynamicGoo *gooA,*gooB;
    gooA=new DynamicGoo(world,ga,RADIUS,this);
    gooB=new DynamicGoo(world,gb,RADIUS,this);
    //Connect slots
    connect(gooB,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
    connect(gooA,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));


    goos.push_back(gooA);
    goos.push_back(gooB);

    makeJoint(gooA,gooB);

    if (flag==DEBUG) qWarning()<<"First joint created, at:"<<a;

    //    connect(gooA,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
    //    connect(gooA,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
    //    connect(gooA,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
    //    connect(gooA,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
    //    connect(gooA,SIGNAL(stopDragging()),this,SLOT(stopDragging()));
    //    connect(gooB,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
    //    connect(gooB,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
    //    connect(gooB,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
    //    connect(gooB,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
    //    connect(gooB,SIGNAL(stopDragging()),this,SLOT(stopDragging()));
}

void Level::createSticky(QPoint p){
    Goo* goo=dynamic_cast<Goo*>(sender());
    if (goo!=NULL){
        QPair <Goo*,QPoint> ps;
        ps.first=goo;
        ps.second=p;
        stickyToCreate.push_back(ps);
    }
}

void Level::destroySticky(){
    //CAST THE SENDER
    StickyLink * sl=dynamic_cast<StickyLink*>(sender());
    if (sl!=NULL){
        //REMOVE STICKY FORM THE LIST
        stickys.removeOne(sl);
        //RETRIVE GOO LINKED AT THE JOINT
        DynamicGoo*dg=dynamic_cast<DynamicGoo*>(sl->getGoo());
        //UNSTICK IT
        dg->unstick();
        //PHISICAL DESTROY THE JOINT
        world->DestroyJoint(sl->getJoint());
        //CLEAN THE MEMORY
        delete sl;
    }
}

//Function to wakeup (maybe) the poor sleeping goos!
void Level::checkForNeighbors(QPoint p){
    //Cast the sender
    Goo* goo=dynamic_cast<Goo*>(sender());
    //check if is realy a goo that call this function
    //we don't want liars!
    if (goo){
        //vector for the distance
        b2Vec2 d;
        //start to cycle all the goos
        for (int i=0;i<goos.count();i++){
            //check if the goo have link and is not himself
            if (goos[i]!=goo && goos[i]->hasJoint()){
                //compute the distance
                d=toVec(p)-goos[i]->getVPosition();
                //check if the distance is less then 2 radius than wakeup goo and return!
                if (d.Length()<goos[i]->getRadius()*3.5) {
                    goo->neighborsFound();
                    return;
                }
            }
        }


    }
}

//Function to stop dragging goo
void Level::stopDragging(){
    //Clear dragging variable
    dragged=NULL;
    drag=false;
    possibility.clear();
    mooving=false;
}

void Level::backToMainMenu()
{
    this->close();//Close the current level
    emit this->eventBackToMainMenu();
}
