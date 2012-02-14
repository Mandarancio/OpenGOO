#include "dynamicgoo.h"
#include "tools.h"


DynamicGoo::DynamicGoo(b2World *world, QPoint p, int radius,  QObject *parent):
    Goo(radius,parent)
{
    this->world=world; //get a copy of the world
    sticked=false; //start not sticked at ground

    b2BodyDef def; //body definition
    def.awake=true; //is active
    def.type=b2_dynamicBody; //is dynamci (react at force impulse and collision)
    def.position=toVec(p); //set the position
    body=world->CreateBody(&def); //create the physical body
    b2CircleShape shape; //define the shape
    shape.m_p.SetZero(); //position of the shape
    shape.m_radius=radius; //radius
    b2FixtureDef fixDef; //Definition of the phisical parameters
    fixDef.restitution=0.3; //collision restitution
    fixDef.density=0.0; //density
    fixDef.friction=0.8; //friction
    fixDef.shape=&shape; //assign the shape
    fixDef.userData=this; //assign a copy of  the object at the body so during the contact is possible to know the info of the goo
    body->CreateFixture(&fixDef); //create the fixture
    body->SetLinearDamping(0.1);//Not sure about this parameter
//    b2MassData* mass=new b2MassData();
//    mass->center.SetZero();
//    mass->I=0.;
//    mass->mass=10.0 ;
//    body->SetMassData(mass);
    moovable=true; //flags
    dragable=true;
    maxJoints=7; //parameters
    speed=50;
}

void DynamicGoo::catched(){
    speed*=2;
}

void DynamicGoo::lost(){
    speed/=2;
}

void DynamicGoo::moveToTarget(){
    if (dragging) return;
    if (hasJoint()) return;
    if (!hasJoint() &&!falling&&!following) emit this->nextTargetPlease(NULL);
    if (following && !falling){
        if (prevTarget){
//            if (!target->isLinked(prevTarget)){
//                drop();
//                return;
//            }
            QPoint meTarget=target->getPPosition()-getPPosition();
            if (toVec(meTarget).Length()>150) {
                stopFollow();
                fallDown();
                return;
            }

            QPoint mePrev=getPPosition()-prevTarget->getPPosition();

            QPoint targetPrev=target->getPPosition()-prevTarget->getPPosition();
            if (abs(mePrev.x())>abs(targetPrev.x())+10 || abs(mePrev.y())>abs(targetPrev.y())+10){
                stopFollow();
                fallDown();
                return;
            }
        }
        b2Vec2 dP;
        if (target!=NULL /*&& target->getBody()!=NULL*/)
            dP=target->getVPosition()-this->getVPosition();
        else {
            stopFollow();
            fallDown();
            return;
        }
        if (dP.Length()<=4){
            emit this->nextTargetPlease(target);
            body->SetLinearVelocity(b2Vec2(0,0));
            return;
        }
        dP.x=dP.x*speed/dP.Length();
        dP.y=dP.y*speed/dP.Length();
        body->SetLinearVelocity(dP);
    }
}


void DynamicGoo::paint(QPainter &p){
    moveToTarget();
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
}

void DynamicGoo::paintDebug(QPainter &p){
    p.setPen((hasJoint()? Qt::green : Qt::white));
    p.setBrush(Qt::transparent);
    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
    if (hasJoint()){
        p.drawText(QPoint(getPPosition().x()-5,getPPosition().y()+5),QString::number(this->nJoints()));
    }
    else if (target!=NULL){
        p.save();
        p.translate(getPPosition());
        p.rotate(atan2(target->getPPosition().y()-getPPosition().y(),target->getPPosition().x()-getPPosition().x())*180.0/3.141628);
        p.drawLine(0,0,40,0);
        p.save();
        p.translate(40,0);
        p.rotate(45);
        p.drawLine(0,0,-5,0);
        p.restore();
        p.save();
        p.translate(40,0);
        p.rotate(-45);
        p.drawLine(0,0,-5,0);
        p.restore();

        p.restore();
    }
}

void DynamicGoo::unstick(){
    sticked=false;
}

void DynamicGoo::contactGround(){
    onGround=true;
    groundPoint=this->getPPosition();
    if (falling) {
        falling=false;
        emit nextTargetPlease(NULL);
    }
}

void DynamicGoo::contactGround(QPoint p){
    onGround=true;
    groundPoint=this->getPPosition();
    if (falling) {
        falling=false;
        emit nextTargetPlease(NULL);
    }
    else if (hasJoint() && !sticked){ //if has joint and is not sticked on ground
        //if (body->GetLinearVelocity().Length()<=90){

        emit this->createSticky(p);
        sticked=true; //flag to true
        //}
    }
}

bool DynamicGoo::isDragable(){
    if (!hasJoint()) return true;
    else return false;
}
