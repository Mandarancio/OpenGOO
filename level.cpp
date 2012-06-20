#include "level.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRadialGradient>
#include <QResizeEvent>


#include "fixedgoo.h"
#include "dynamicgoo.h"
#include "removablegoo.h"
#include "balloongoo.h"
#include "stickygoo.h"
#include "thorn.h"
#include "stickylink.h"


#include "balloongoo.h"
#include "ropejoint.h"
#include "collisionlistener.h"

#include "flags.h"

#include <QPolygon>
#include <QTextFormat>

#define RADIUS 18
#define INTERVALL 4
#define DELAY 10


Level::Level(QString level,BackGroundWidget *bg,QWidget *parent) :
    QWidget(parent), backGroundWidget(bg)
{
    scale=1.0;
    goal = 100;

    //set the display geometry
  //  this->setGeometry(0,0,geometry.width(),geometry.height());

    //if (flag & DEBUG) qWarning()<<"Geometry setted:"<<geometry;


    //grab keyboard, mouse and track it!
    this->grabKeyboard();
    this->grabMouse();
    this->setMouseTracking(true);
    if (flag & DEBUG) qWarning()<<"Mouse and keyboard grabbed";

    initialize();

    //setup the leveloader with some enviroment parameters
    loader=new SvgLevelLoader(level);

    //connect the loader signals
    connect(loader,SIGNAL(fileError()),this,SLOT(backToMainMenu()));
    connect(loader,SIGNAL(levelName(QString)),this,SLOT(setName(QString)));
    connect(loader,SIGNAL(levelGeometry(QSize)),this,SLOT(setLevelGeometry(QSize)));
    connect(loader,SIGNAL(levelGoal(int)),this,SLOT(setGoal(int)));
    connect(loader,SIGNAL(levelGround(QPoint,QList<QPoint>)),this,SLOT(setGround(QPoint,QList<QPoint>)));
    connect(loader,SIGNAL(levelLimit(QRect)),this,SLOT(setLimit(QRect)));
    connect(loader,SIGNAL(levelTarget(QPoint)),this,SLOT(setTarget(QPoint)));
    connect(loader,SIGNAL(levelJoint(Goo*,Goo*)),this,SLOT(setJoint(Goo*,Goo*)));
    connect(loader,SIGNAL(levelGOO(QPoint,int,int)),this,SLOT(setGoo(QPoint,int,int)));
    connect(loader,SIGNAL(addBackGroundShape(int,QPolygon,QColor)),this,SLOT(addBGShape(int,QPolygon,QColor)));
    connect(loader,SIGNAL(addLevelThorn(QPoint,QList<QPoint>)),this,SLOT(setThorns(QPoint,QList<QPoint>)));

    connect(loader,SIGNAL(levelStartArea(int,QRect,int)),this,SLOT(setStartArea(int,QRect,int)));
    if (flag & DEBUG) qWarning()<<"Level loader created, set up and connected!";



    //setup the step variable
    //this one is the interval between step
    step=1.0/40.0;
    //initialize variables for draggin goo
    drag = false;
    dragged=NULL;
    selected=NULL;
    target=NULL;


    showJointTimer=0;

    points=0;
    catched=false;
    if (flag & DEBUG) qWarning()<<"Game variable initialized!";


    menu=new Menu(this->geometry(),this);
    onMenu=false;
    mooving=false;
    connect(menu,SIGNAL(eventClose()),this,SLOT(closeAll()));
    connect(menu,SIGNAL(eventResume()),this,SLOT(resume()));
    connect(menu,SIGNAL(eventRestart()),this,SLOT(restart()));
    connect(menu,SIGNAL(eventBackToMainMenu()),this,SLOT(backToMainMenu()));    
    if (flag & DEBUG) {
        qWarning()<<"Menu set up!";
        uint32 flags = 0;//Set the flags
        flags += 1* b2Draw::e_shapeBit;
        flags += 1* b2Draw::e_jointBit;


        debugPainter= new QB2Draw(this->geometry());
        debugPainter->SetFlags(flags);
        world->SetDebugDraw(debugPainter);
    }


}

//Function to start the level
//parse level file
//start timer
bool Level::startLevel(){
    //load the level
    if (loader->parse()){
        if (flag & DEBUG) qWarning()<<"Level parse finished!";
        //start timer
        startTimer(step*1000);
        time= 0;
        step=1.0/30.0;
        if (flag & DEBUG) qWarning()<<"Timer started!"<<"Time step is:"<<step<<"second";
        return true;
    }
    else return false;
}


//Initialize function
void Level::initialize()
{



    dir.left=false;
    dir.right=false;
    dir.key=false;
    dir.up=false;
    dir.down=false;
    //compute the center of the display
    //Fix for multi screen
    center=QPoint(0,0);//(geometry.x() == 0 ? geometry.center() : geometry.center()-geometry.topLeft());
    //initialize translation values
    translation=QPoint(0,0);

    //create world
    //b2vec2(0,2000) is the gravity force
    world = new b2World(b2Vec2(0,20.0));
    world->SetAutoClearForces(true);
    world->SetContinuousPhysics(true);
    world->SetAllowSleeping(true);
    if (flag & DEBUG) qWarning()<<"World object created!";

    //setup our modified collisionlistener
    CollisionListener *cl=new CollisionListener(this);
    world->SetContactListener(cl);
    connect(cl,SIGNAL(stopGOO(QPoint)),this,SLOT(stopGoo(QPoint)));
    if (flag & DEBUG) qWarning()<<"Collision listener created and set up!";

    //setup the step variable
    //this one is the interval between step
    step=1.0/60.0;
    //initialize variables for draggin goo
    drag = false;
    dragged=NULL;
    selected=NULL;
    target=NULL;


    showJointTimer=0;

    points=0;
    catched=false;
    if (flag & DEBUG) qWarning()<<"Game variable initialized!";

    if (flag & DEBUG) {
        qWarning()<<"Menu set up!";
        uint32 flags = 0;//Set the flags
        flags += 1* b2Draw::e_shapeBit;
        flags += 1* b2Draw::e_jointBit;


        debugPainter= new QB2Draw(this->geometry());
        debugPainter->SetFlags(flags);
        world->SetDebugDraw(debugPainter);
    }
}
//Clean function
void Level::clean(){
    //clear object bodies
    backGroundWidget->clear();
    for (int i=0;i<objects.length();i++){
        world->DestroyBody(objects[i]->getBody());
        delete objects[i];
    }
    objects.clear();
    if (flag & DEBUG) qWarning()<<"Objects deleated";
    //clear joints
    for (int i=0;i<joints.length();i++){
        world->DestroyJoint(joints[i]->getJoint());
        delete joints[i];
    }
    joints.clear();
    if (flag & DEBUG) qWarning()<<"Joints deleated";
    //clear stickies;
    for (int i=0;i<stickys.length();i++){
        world->DestroyJoint(stickys[i]->getJoint());
        delete stickys[i];
    }
    stickys.clear();
    if (flag & DEBUG) qWarning()<<"Stickys deleated";
    //clear goo body
    for (int i=0;i<goos.length();i++){
        world->DestroyBody(goos[i]->getBody());
        delete goos[i];
    }
    goos.clear();
    if (flag & DEBUG) qWarning()<<"GOOs deleated";
    //clear ground.
    for (int i=0;i<ground.length();i++){
        world->DestroyBody(ground[i]->getBody());
        delete ground[i];
    }
    ground.clear();
    if (flag & DEBUG) qWarning()<<"Ground deleated";
    //clear world.
    delete world;
    if (flag & DEBUG) qWarning()<<"World deleated";
    if (flag & DEBUG){
        delete debugPainter;
        qWarning()<<"Debug painter deleted";
    }
}

//RETRIVE THE JOINT  (IF ONE) UNDER THE GOO

Joint* Level::overJoint(Goo *goo){
    int r=qRound(goo->getRadius()/1.41);
    QRect rect(goo->getPPosition()-QPoint(r,r),QSize(r*2,r*2));
    QPolygon pol(rect);
    for (int i=0;i<joints.length();i++){
        if (joints[i]->goo(true)->canHaveGuest() && joints[i]->goo(false)->canHaveGuest() && joints[i]->boundingRect().intersects(rect) && joints[i]->boundingPoly().intersected(pol).count()) return joints[i];
    }
    return NULL;
}

//retrive a goo in a point (if there is a goo)
Goo* Level::getGooAt(QPoint p){
    b2Vec2 d;
    for (int i=goos.count()-1;i>=0;i--){
        if (goos[i]->isMoovable()&&goos[i]->isDragable()){
            d=b2Vec2(p.x(),p.y())-goos[i]->getVPositionScaled();
            if (d.Length()<goos[i]->getRadius()*scale) return goos[i];
        }
    }
    return NULL;
}


//Function to translate the scene
void Level::moveUp(){
    int xf=translation.x();
    int yf=translation.y()+10+positionTimer;
    QRect view(-xf,-yf,width(),height());
    if (!limit.contains(view)) return;
    translation=QPoint(xf,yf);
    backGroundWidget->translated(translation);
}


void Level::moveDown(){
    int xf=translation.x();
    int yf=translation.y()-10-positionTimer;
    QRect view(-xf,-yf,width(),height());
    if (!limit.contains(view)) return;
    translation=QPoint(xf,yf);
    backGroundWidget->translated(translation);
}
void Level::moveRight(){
    int xf=translation.x()-10-positionTimer;
    int yf=translation.y();
    QRect view(-xf,-yf,width(),height());
    if (!limit.contains(view)) return;
    translation=QPoint(xf,yf);
    backGroundWidget->translated(translation);
}
void Level::moveLeft(){
    int xf=translation.x()+10+positionTimer;
    int yf=translation.y();
    QRect view(-xf,-yf,width(),height());
    if (!limit.contains(view)) return;
    translation=QPoint(xf,yf);
    backGroundWidget->translated(translation);
}

void Level::moveOf(QPoint dP){
    int xf,yf;
    //top left coordinate of the new view field
    xf=translation.x()+dP.x()/2;
    yf=translation.y()+dP.y()/2;
    //view field rectange
    QRect view(-xf,-yf,width(),height());
    //Check if is possible
    if (!limit.contains(view)) return;
    translation=QPoint(xf,yf);
    if (dP.x()/2 || dP.y()/2)
        backGroundWidget->translated(translation);
}

//Create a joint
bool Level::makeJoint(Goo *a, Goo *b){
    bool baloon=false;
    if (a->getType()==BALOON ||b->getType()==BALOON) baloon=true;
    if (!a->createLink(b)) return false;
    if (!b->createLink(a)) {
        a->destroyLink(b);
        return false;
    }
    if (!baloon){
        Joint* j= new Joint(a,b,world,false,this);
        joints.push_back(j);
        connect(j,SIGNAL(destroyJoint(Joint*)),this,SLOT(destroyJoint(Joint*)));

    }
    else {
        RopeJoint *rj=new RopeJoint(b,a,world,this);
        joints.push_back(rj);
        connect(rj,SIGNAL(destroyJoint(Joint*)),this,SLOT(destroyJoint(Joint*)));
    }
    return true;
}

QList<Goo*> Level::possibleJoints(QPoint p){
    QList<Goo*> l;
    b2Vec2 pv=toVec(p);
    b2Vec2 d;
    if (overJoint(dragged)!=NULL) return l;
    for (int i=0;i<goos.length();i++){
        if (goos[i]->canHaveJoint()) {
            d=pv-goos[i]->getVPosition();
            if (goos[i]!=dragged && d.Length()<dragged->getRadius()/10.0) {
                l.clear();
                return l;
            }
            if (d.LengthSquared()>=5*5 && d.LengthSquared()<=dragged->getDistanceToJoint()/10.0*dragged->getDistanceToJoint()/10.0)
                l.push_back(goos[i]);

        }
    }
    return l;
}

bool Level::createJoints(QPoint p){

    QList<Goo*> l;
    l=possibleJoints(p);
    if (dragged->getType()!=BALOON){
        if (l.length()>1||dragged->hasJoint()){
            for (int i=0;i<l.length();i++){
                if (makeJoint(dragged,l[i])) continue;
            }
            return true;
        }
        else return false;
    }
    else {
        if (l.length() && !dragged->hasJoint()){

            makeJoint(dragged,getNearest(dragged->getPPosition(),l));
            return true;
        }
        else return false;
    }

}

void Level::anchorToJoint(Goo *goo, Joint *j){
    if (j->goo(true)->canHaveGuest() && j->goo(false)->canHaveGuest()){
        bool choise=qrand()%2;
        goo->setTarget(j->goo(!choise));
        goo->setTarget(j->goo(choise));
    }
}

void Level::timerEvent(QTimerEvent *e){
    if (points<goal)
        time+=step;
    e->accept();

    if (drag) showJointTimer++;
    if (drag) {

        dragged->getBody()->SetActive(true);
        if ((dragged->getVPosition()-0.1*mousePos+toVec(translation)).Length()>3.0){
            draggTimer++;
            b2Vec2 force=(0.1*mousePos-toVec(translation)-dragged->getVPosition());
            force=float(10.0*draggTimer)/force.Length()*force;
            dragged->getBody()->SetLinearVelocity(100000.0*force);

        }
        else if (!groundContains(toPoint(0.1*mousePos)-translation,5)){
            draggTimer=0;
            dragged->move(toPoint(0.1*mousePos)-translation);
        }
    }
    if (!onMenu){
        if (dir.left) {
            QPoint p=translation;
            moveLeft();
            p=p-translation;
            positionTimer++;
            //if (drag) dragged->move(dragged->getPPosition()+p);
        }
        else if (dir.right) {
            QPoint p=translation;
            moveRight();
            p=p-translation;
            positionTimer++;
            //if (drag) dragged->move(dragged->getPPosition()+p);
        }
        if (dir.down) {
            QPoint p=translation;
            moveDown();
            p=p-translation;
            positionTimer++;
            //if (drag) dragged->move(dragged->getPPosition()+p);
        }
        else if (dir.up) {
            QPoint p=translation;
            moveUp();
            p=p-translation;
            positionTimer++;
            //if (drag) dragged->move(dragged->getPPosition()+p);
        }
    }
    for (int i=0;i<stickys.length();i++) stickys[i]->checkStatus();
    for (int i=0;i<2;i++)
        world->Step(step/2,10,10);
     if (drag) {
         possibility=possibleJoints(dragged->getPPosition());
         dragged->drag();
     }
    for (int i=0;i<stickyToCreate.length();i++){
        QPair<Goo*,QPoint> p= stickyToCreate.at(i);
        StickyLink*sl=new StickyLink(p.first,ground[0]->getBody(),p.second,world,p.first->getStickness());
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

    if (target) target->checkTower(goos);
    if (target) target->applyForces(goos);

    stickyToCreate.clear();
    repaint();

}

Goo *Level::getNearest(QPoint p,QList<Goo*> l){
    if (l.length()){
        Goo* p0=l.at(0);
        QPoint dp=p-p0->getPPosition();
        float dt,d=dp.x()*dp.x()+dp.y()*dp.y();
        for (int i=0;i<l.length();i++){
            dp=p-l[i]->getPPosition();
            dt=dp.x()*dp.x()+dp.y()*dp.y();
            if (dt<d) {
                d=dt;
                p0=l[i];
            }
        }
        return p0;
    }
    else return NULL;
}

bool Level::groundContains(Goo *goo){
    for (int i=0;i<ground.length();i++)
        if (ground[i]->contains(goo)) return true;
    return false;
}

bool Level::groundContains(QPoint p, int radius){
    for (int i=0;i<ground.length();i++)
        if (ground[i]->contains(p,radius)) return true;
    return false;
}




void Level::paintEvent(QPaintEvent *e){

    QPainter p(this);
    //BG Color


    p.setPen(Qt::transparent);
    p.setBrush(Qt::darkGray);


    p.setRenderHint(QPainter::Antialiasing);

    QRect display(-translation,QSize(width(),height()));

    p.save();
    p.scale(scale,scale);
    p.translate(translation);


    for (int i=0;i<objects.length();i++){
        if (display.intersects(objects[i]->boundingRect()) || (flag & OPENGL))
            objects[i]->paint(p);

    }
    for (int i=0;i<joints.length();i++) {
        if (joints[i] ) {
            joints[i]->status();
            if ( display.intersects(joints[i]->boundingRect()) || (flag & OPENGL)){
                joints[i]->paint(p);
                if ((flag & DEBUG) && !(flag & ONLYTEXT)){
                    joints[i]->paintDebug(p);
                }
            }
        }
    }
    for (int i=0;i<goos.length();i++){
        if (goos[i]  && goos[i]->hasJoint() && !goos[i]->isDragging() && !goos[i]->isSelected()){
            goos[i]->update();
            if ( display.intersects(goos[i]->boundingRect()) || (flag & OPENGL)){
                goos[i]->paint(p);
                if ((flag & DEBUG) && !(flag & ONLYTEXT)){
                    goos[i]->paintDebug(p);
                }
            }
        }
    }
    for (int i=0;i<goos.length();i++){
        if (goos[i] && !goos[i]->isDragging() && !goos[i]->isSelected() && !goos[i]->hasJoint()) {
            goos[i]->update();
            if (goos[i]->isFalling() && overJoint(goos[i])!=NULL) anchorToJoint(goos[i],overJoint(goos[i]));
            if (display.intersects(goos[i]->boundingRect()) || (flag & OPENGL)){
                goos[i]->paint(p);
                if ((flag & DEBUG) && !(flag & ONLYTEXT)){
                    goos[i]->paintDebug(p);
                }
            }
        }
    }

    if ((flag & DEBUG) && !(flag & ONLYTEXT)) {
        for (int i=0;i<stickys.length();i++)
            stickys[i]->paint(p);
    }

    //Draw the center of the map
    if ((flag & DEBUG) && !(flag & ONLYTEXT)){
        p.setPen(Qt::white);
        p.drawLine(0,5,0,-5);
        p.drawLine(5,0,-5,0);
    }

    //Draw selected and dragged goo
    if (dragged!=NULL) {
        p.setPen(Qt::yellow);

        if ((flag & DEBUG) && overJoint(dragged)!=NULL) {
            p.setBrush(Qt::transparent);
            p.drawRect(overJoint(dragged)->boundingRect());
        }
        if ((dragged->getType()!=BALOON &&possibility.length()>1) && (showJointTimer>DELAY))
        {
            for (int i=0;i<possibility.length();i++)
                p.drawLine(dragged->getPPosition(),possibility[i]->getPPosition());
        }
        else if (drag && dragged->getType()==BALOON){
            if (getNearest(dragged->getPPosition(),possibility)!= NULL){
                p.drawLine(dragged->getPPosition(),getNearest(dragged->getPPosition(),possibility)->getPPosition());
            }
        }
        p.setPen(Qt::black);
        dragged->paint(p);
        if ((flag & DEBUG) && !(flag & ONLYTEXT)){
                dragged->paintDebug(p);
        }
    }
    if (selected!=NULL){
        selected->update();
        selected->paint(p);
        if ((flag & DEBUG) && !(flag & ONLYTEXT)){
                selected->paintDebug(p);
        }
        float d=toVec(selected->getPPosition()-absoluteMousePos-translation).Length()*10;
        if (d>selected->getRadius()+10){
            selected->select(false);
            selected=getGooAt(absoluteMousePos/scale-translation);
            if (selected!=NULL) selected->select();
        }
    }

    if ((flag & DEBUG) && !(flag & ONLYTEXT)){

        debugPainter->setPainter(&p);

        world->DrawDebugData();
    }
    p.restore();

    //Draw the center of the display
    p.save();
    p.translate(center);

    if (flag & DEBUG){
        p.setPen(Qt::yellow);
        p.drawLine(0,5,0,-5);
        p.drawLine(5,0,-5,0);
    }

    p.setBrush(Qt::darkGray);
    p.restore();

    //Paint the targetarrow the win screen menu and etc...
    paintTargetArrow(p);
    if (onMenu) menu->paint(p);
    paintWin(p);
    paintScore(p);
    paintButton(p);

//    p.beginNativePainting();

//    glEnable(GL_SCISSOR_TEST);
//    glScissor(0, 0, 64, 64);

//    glClearColor(1, 0, 0, 1);
//    glClear(GL_COLOR_BUFFER_BIT);

//    glDisable(GL_SCISSOR_TEST);

//    glEnd();

//    p.endNativePainting();

    if (p.end()) e->accept();

    else e->ignore();
}

void Level::keyReleaseEvent(QKeyEvent *e){
    switch(e->key()){
    case (Qt::Key_Escape):
        onMenu=!onMenu;
        break;
    case (Qt::Key_Up):
        dir.up=false;
        dir.key=false;
        break;
    case (Qt::Key_Down):
        dir.down=false;
        dir.key=false;
        break;
    case (Qt::Key_Left):
        dir.left=false;
        dir.key=false;
        break;
    case (Qt::Key_Right):
        dir.right=false;
        dir.key=false;
        break;
    }
}

void Level::keyPressEvent(QKeyEvent *e){
    switch (e->key()){
    case (Qt::Key_Up):
        dir.up=true;
        dir.key=true;
        this->positionTimer=0;
        break;
    case (Qt::Key_Down):
        dir.down=true;
        dir.key=true;
        this->positionTimer=0;
        break;
    case (Qt::Key_Left):
        dir.left=true;
        dir.key=true;
        this->positionTimer=0;
        break;
    case (Qt::Key_Right):
        dir.right=true;
        dir.key=true;
        this->positionTimer=0;
        break;
    }
}

//~700,~900
void Level::mouseMoveEvent(QMouseEvent *e){
   // absoluteMousePos=e->pos();
    if (onMenu){
        return;
    }
    if (e->x()<=20) {
        dir.left=true;
        this->positionTimer=0;
    }
    else if (dir.left && !dir.key)
        dir.left=false;

    if (e->y()<=20) {
        dir.up=true;
        this->positionTimer=0;
    }
    else if (dir.up && !dir.key)
        dir.up=false;

    if (e->x()>=width()-20){
        dir.right=true;
        this->positionTimer=0;
    }
    else if (dir.right && !dir.key)
        dir.right=false;

    if (e->y()>=height()-5){
        dir.down=true;
        this->positionTimer=0;
    }
    else if (dir.down && !dir.key)
        dir.down=false;

    if (drag){
        //compute the mouse speed (so when the goo is released it get the mouse spped)
        mouseSpeed=(10*toVec(e->pos())-mousePos);
        mouseSpeed.x*=10;
        mouseSpeed.y*=10;

        mousePos=10*toVec(e->pos());

        /*
        bool same=true;
        if (possibility.length()!=possibleJoints()*/
        if (possibleJoints(dragged->getPPosition()).length()==0 || possibleJoints(dragged->getPPosition()).length()!=possibility.length()) showJointTimer=0;
        //Check for possibles joints
        possibility=possibleJoints(dragged->getPPosition());
    }
    //Rutine for translate the scene
    else if (mooving) {
        QPoint d=e->pos()-toPoint(mousePos)/10;
        mousePos=10*toVec(e->pos());
        moveOf(d);
    }
    //Rutine to show the possible draggable go under the mouse
    else if (points<goal){
        if (selected!=getGooAt(e->pos()/scale-translation)){

            if (selected!=NULL) {
                //Unselect old and select new.
                selected->select(false);
                selected=getGooAt(e->pos()/scale-translation);
                if (selected!=NULL) selected->select();
            }
            else {
                //select new
                selected=getGooAt(e->pos()/scale-translation);
                if (selected!=NULL) selected->select();
            }
        }
    }

}
void Level::mousePressEvent(QMouseEvent *e){
    if (onMenu || points>=goal) return;
    if (e->button()==Qt::LeftButton ) {
        //Compute mouse speed
        mousePos=10*toVec(e->pos());
        mouseSpeed.SetZero();

        //If no goo is selected
        if (selected==NULL){
            //Get the goo in this position
            dragged=getGooAt(e->pos()/scale-(center+translation));
            //Rutine to drag a goo.
            if (dragged) {
                possibility.clear();
                drag=true;
                draggTimer=0;
                dragged->drag();
                dragged->move(e->pos()/scale-translation);

            }
            else mooving=true;
        }
        else {
            b2Vec2 d=toVec(e->pos()/scale-(center+translation))-selected->getVPosition();
            //Check if selected is near the mouse
            //if not deselect and get a new goo
            if (d.Length()>16){
                selected->select(false);
                selected=NULL;
                //Get the goo in this position
                dragged=getGooAt(e->pos()/scale-(center+translation));

                //Rutine to drag a goo.
                if (dragged) {
                    possibility.clear();
                    drag=true;
                    dragged->drag();
                    dragged->move(e->pos()/scale-translation);

                }
                else mooving=true;
            }
            else {
                dragged=selected;
                possibility.clear();
                drag=true;
                dragged->drag();
                dragged->move(e->pos()/scale-translation);
                selected->select(false);
                selected=NULL;
            }

        }
    }
}


void Level::mouseReleaseEvent(QMouseEvent *e){
    if (e->button()!=Qt::LeftButton) return;
    if (!drag){
        clickButton(e->pos());
    }
    else if (drag){
        if (overJoint(dragged)!=NULL){
            dragged->drop(b2Vec2(0,0));
            anchorToJoint(dragged,overJoint(dragged));
        }
        else if (showJointTimer<=DELAY){
            dragged->drop(10.0*mouseSpeed+10.0*toVec(e->pos()-translation-dragged->getPPosition()));
        }
        else if (createJoints(dragged->getPPosition()) || dragged->hasJoint()) dragged->drop();
        else dragged->drop(10.0*mouseSpeed+10.0*toVec(e->pos()-translation-dragged->getPPosition()));
    }
    dragged=NULL;
    drag=false;
    possibility.clear();
    mooving=false;
    showJointTimer=0;



    if (onMenu){
        menu->mouseRelease(e);
        return;
    }
}


void Level::resizeEvent(QResizeEvent *e){
    menu->setGeometry(QRect(0,0,e->size().width(),e->size().height()));
}

void Level::destroyJoint(Joint *joint){

    jointsToDestroy.push_back(joint);
}

void Level::giveTarget(Goo *previous){
    Goo *goo=dynamic_cast<Goo*>(sender());
    if (goo!=NULL){
        if (!previous){
            Goo * next=NULL ;
            bool ok=false;
            float distance=60;
            if (!goo->isOnGround()) return;
            for (int i=0;i<goos.length();i++){
                if (goos[i]!=goo && goos[i]->hasJoint() && goos[i]->isOnGround() && (goos[i]->getVPosition()-goo->getVPosition()).Length()<=distance){
                    float my=(goos[i]->getVPosition().y-goo->getVPosition().y)/(goos[i]->getVPosition()-goo->getVPosition()).Length();
                    if (my>-1 && my<0.5) {

                        next=goos[i];
                        distance=(goo->getVPosition()-goos[i]->getVPosition()).Length();
                        ok=true;
                    }

                }
            }
            if (ok) goo->setTarget(next);
        }
        else {
            if (!catched && previous!=NULL && previous->getLinks().length()){
                int choise=qrand()%previous->getLinks().length();
                goo->setTarget(previous->getLinks().at(choise));
            }
            else if (previous->getLinks().length()) {
                //Searche the nearest next goo at the target
                Goo * target=previous->getLinks().at(0);
                b2Vec2 d=this->target->getVPosition()-target->getVPosition();
                for (int i=1;i<previous->getLinks().length();i++){
                    if ((goo->getPrevious()!=previous->getLinks().at(i))  && (this->target->getVPosition()-previous->getLinks().at(i)->getVPosition()).LengthSquared()<d.LengthSquared()){
                        target=previous->getLinks().at(i);
                        d=this->target->getVPosition()-previous->getLinks().at(i)->getVPosition();
                    }
                }
                if (target==previous){
                    qWarning()<<"ERROR";

                }
                goo->setTarget(target);
            }
            else {
                // qWarning()<<"Here!!";
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


void Level::paintScore(QPainter &p){

    QFont f;
    f.setFamily("Helvetica [Cronyx]");
    f.setBold(true);
    f.setPixelSize(44);

    QPainterPath path;


    QFontMetrics fm(f);
    path.addText(10,height()-26,f,QString::number(points));

    f.setPixelSize(22);
    path.addText(10+fm.width(QString::number(points)),height()-14,f,QString::number(goal));
    f.setPixelSize(30);
    path.addText(10,30,f,time2string(time));



    p.setPen(QPen(Qt::black,2));
    p.setBrush(Qt::white);
    p.drawPath(path);

}

//Function to paint the win screen
void Level::paintWin(QPainter &p){
    if (points>=goal){
        //if the menu is hidden draw the semi-black background
        if (!onMenu){
            QColor bg(0,0,0,200);
            p.setBrush(bg);
            p.setPen(bg);
            p.drawRect(0,0,width(),height());
        }
        //Setup fonts and write win on the screen
        QFont f;
        f.setFamily("Times");
        f.setBold(true);
        f.setPointSize(32);
        p.setFont(f);
        p.setPen(Qt::white);
        QRect r(0,0,width(),150);
        p.drawText(r,Qt::AlignCenter|Qt::AlignHCenter,name+" complited!");
    }
}
//Function to paint the target arrow
void Level::paintTargetArrow(QPainter &p){
    //Check if the target is displayed:
    if (!target) return;
    QPoint tp= toPoint(target->getVPosition());

    QRect darea(-translation,this->geometry().size());
    QPoint center=geometry().center()-geometry().topLeft();
    if (!darea.contains(tp)){
        //Target is not displayed.
        tp=translation+tp*scale-center;
        //Translate at the center of the area and rotate in the direction of the target
        p.save();
        p.setBrush(Qt::black);
        p.translate(center);
        p.setPen(Qt::black);
        //Compute the angle of the vector between center and target
        float angle=atan2(static_cast<float>(-tp.y()),static_cast<float>(-tp.x()))*180.0/3.141628-180;
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

void Level::restart(){
    clean();
    initialize();
    loader->parse();
    resume();
    connect(target,SIGNAL(gooCatched(Goo*)),this,SLOT(gooCatched(Goo*)));
    connect(target,SIGNAL(towerCatch()),this,SLOT(towerCatched()));
    connect(target,SIGNAL(towerLost()),this,SLOT(towerLost()));
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

void Level::setGoal(int goal){
    if (flag & DEBUG) qWarning()<<"Goal:"<<goal;
    this->goal=goal;
}

void Level::setLimit(QRect limit){
    if (flag & DEBUG) qWarning()<<"Level limit:"<<limit;
    this->limit=limit;
    float sx=float(width())/float(limit.width());
    float sy=float(height())/float(limit.height());
    if (sx>sy && sx>1.0) scale=sx;
    else if (sy>=sx && sy>1.0) scale=sy;
    this->limit.setTopLeft(limit.topLeft()*scale);
    this->limit.setSize(limit.size()*scale);
    if (scale>1) this->translation=QPoint(-limit.topLeft().x(),limit.topLeft().y());
    backGroundWidget->setLimit(limit);
    backGroundWidget->translated(translation);
    backGroundWidget->setScale(scale);
    if (flag & DEBUG) qWarning()<<"SCALE"<<scale;
}

void Level::setGround(QPoint gCenter, QList<QPoint> gList){
    Ground * g=new Ground(world,gCenter,gList,this);
    backGroundWidget->addGround(g);
    ground.push_back(g);
    if (flag & DEBUG) qWarning()<<"Ground created.";
}

void Level::setTarget(QPoint target){
    if (flag & DEBUG) qWarning()<<"Target at:"<<target;
    this->target=new Target(target,limit.height(),world,this);
    //connect target signals with level
    connect(this->target,SIGNAL(gooCatched(Goo*)),this,SLOT(gooCatched(Goo*)));
    connect(this->target,SIGNAL(towerCatch()),this,SLOT(towerCatched()));
    connect(this->target,SIGNAL(towerLost()),this,SLOT(towerLost()));
    backGroundWidget->setTarget(this->target);
    if (flag & DEBUG) qWarning()<<"Target connected!";

}

void Level::setStartArea(int n, QRect area,int type){
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
                goos.push_back(dg);
                connect(dg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
                connect(dg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
                connect(dg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
                connect(dg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
                connect(dg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
        }
        else if (type==1){ //Create a removable goo
                RemovableGoo* rg=new RemovableGoo(world,QPoint(x,y),radius);
                goos.push_back(rg);
                connect(rg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
                connect(rg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
                connect(rg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
                connect(rg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
                connect(rg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
        }
        else if (type==2){ //Create a fixed goo
                FixedGoo* fg=new FixedGoo(world,QPoint(x,y),radius);
                goos.push_back(fg);
                connect(fg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
                connect(fg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
                connect(fg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
        }
        else if (type==3){ //Create a balloon goo
                BalloonGoo*  bg=new BalloonGoo(world,QPoint(x,y),radius+4);
                goos.push_back(bg);
                connect(bg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
                connect(bg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
                connect(bg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
                connect(bg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
                connect(bg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
        }
        else if (type==4){ //Create a sticky goo
                StickyGoo*  sg=new StickyGoo(world,QPoint(x,y),radius);
                goos.push_back(sg);
                connect(sg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
                connect(sg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
                connect(sg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
                connect(sg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
                connect(sg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
        }
        else return;
    }
    if (flag & DEBUG) qWarning()<<"A start area is created.";
}

void Level::setJoint(Goo*a,Goo*b){


    makeJoint(a,b);

    if (flag & DEBUG) qWarning()<<"First joint created!";
}

void Level::setGoo(QPoint center,int id, int type){
    Goo* goo=NULL;
    int radius=RADIUS+(qrand()%(INTERVALL*2)-INTERVALL);
    if (type==0) { //Create a standard gooo
            DynamicGoo* dg=new DynamicGoo(world,center,radius);
            goo=dg;
            goos.push_back(dg);
            connect(dg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
            connect(dg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
            connect(dg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
            connect(dg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
            connect(dg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
            connect(dg,SIGNAL(stopDragging()),this,SLOT(stopDragging()));
    }
    else if (type==1){ //Create a removable goo
            RemovableGoo* rg=new RemovableGoo(world,center,radius);
            goos.push_back(rg);
            goo=rg;
            connect(rg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
            connect(rg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
            connect(rg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
            connect(rg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
            connect(rg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
    }
    else if (type==2){ //Create a fixed goo
            FixedGoo* fg=new FixedGoo(world,center,radius);
            goos.push_back(fg);
            goo=fg;
            connect(fg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
            connect(fg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
            connect(fg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
    }
    else if (type==3){ //Create a balloon goo
            BalloonGoo*  bg=new BalloonGoo(world,center,radius+4);
            goos.push_back(bg);
//            ballGoos.append(bg);
            goo=bg;
            connect(bg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
            connect(bg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
            connect(bg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
            connect(bg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
            connect(bg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
    }
    else if (type==4){ //Create a sticky goo
            StickyGoo*  sg=new StickyGoo(world,center,radius);
            goos.push_back(sg);
            goo=sg;
            connect(sg,SIGNAL(nextTargetPlease(Goo*)),this,SLOT(giveTarget(Goo*)));
            connect(sg,SIGNAL(destroyGoo()),this,SLOT(destroyGOO()));
            connect(sg,SIGNAL(destroyJoint(Goo*,Goo*)),this,SLOT(destroyJoint(Goo*,Goo*)));
            connect(sg,SIGNAL(createSticky(QPoint)),this,SLOT(createSticky(QPoint)));
            connect(sg,SIGNAL(checkForNeighbors(QPoint)),this,SLOT(checkForNeighbors(QPoint)));
    }
    if (goo!=NULL){
        loader->addGoo(id,goo);
    }
}

void Level::setLevelGeometry(QSize size){
    if (flag & DEBUG) qWarning()<<"Level size"<<size;
}

void Level::setThorns(QPoint center, QList<QPoint> list){
    Thorn *thorn=new Thorn(center,list,*world,this);
    objects.push_back(thorn);
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
        if (dg==NULL) return;
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
                //check if the distance is less then 10 radius than wakeup goo and return!
                if (d.Length()<goos[i]->getRadius()) {
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

//function to stop a goo
void Level::stopGoo(QPoint p){
    /*if (groundContains(p,dragged->getRadius())){
        if (flag & DEBUG) qWarning()<<"P is in the Ground, stop dragging ";
        stopDragging();
    }
    else stopPosition = p;*/
}

void Level::addBGShape(int level, QPolygon poly, QColor color){
    int index=-1;
    for (int i=0;i<background.length();i++){
        if (background[i]->getLevel()==level) {
            index=i;
            break;
        }
    }
    if (index>=0){
        background[index]->addPolygon(poly,color);
    }
    else {
        BackGround *bg=new BackGround(level,this);
        backGroundWidget->addBackGround(bg);
        bg->addPolygon(poly,color);
        bg->setDelta(0.3*(3-level));
        background.push_back(bg);
    }
}
