#include "joint.h"
#include "tools.h"

Joint::Joint(Goo *a, Goo *b, b2World *world,bool child, QObject *parent):
    QObject(parent)
{

    this->a=a;
    this->b=b;
    type=NORMAL;
    if (!child) initialize(world);
}

void Joint::initialize(b2World * world){
    b2DistanceJointDef jDef;
    jDef.Initialize(a->getBody(),b->getBody(),a->getVPosition(),b->getVPosition());
    jDef.dampingRatio=1.0;
    jDef.frequencyHz=10;
    jDef.collideConnected=true;
    joint=(b2DistanceJoint*)world->CreateJoint(&jDef);
}

void Joint::paint(QPainter &p){
    status();
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(4);
    p.setPen(pen);
    QPoint a=toPoint(joint->GetBodyA()->GetPosition());
    QPoint b=toPoint(joint->GetBodyB()->GetPosition());
    p.drawLine(a,b);
    p.drawLine(a.x()-2,a.y()-2,b.x()+2,b.y()+2);
    p.drawLine(a.x()-1,a.y()-1,b.x()+1,b.y()+1);
    p.drawLine(a.x()-3,a.y()-3,b.x()+3,b.y()+3);
    p.drawLine(a.x()-4,a.y()-4,b.x()+4,b.y()+4);
    p.drawLine(a.x()+4,a.y()+4,b.x()-4,b.y()-4);
    p.drawLine(a.x()+3,a.y()+3,b.x()-3,b.y()-3);
    p.drawLine(a.x()+1,a.y()+1,b.x()-1,b.y()-1);
    p.drawLine(a.x()+2,a.y()+2,b.x()-2,b.y()-2);
}

void Joint::paintDebug(QPainter &p){
    p.setPen(Qt::green  );
    p.drawLine(toPoint(joint->GetBodyA()->GetPosition()),toPoint(joint->GetBodyB()->GetPosition()));
    QPoint m=(toPoint(joint->GetBodyA()->GetPosition())+toPoint(joint->GetBodyB()->GetPosition()))/2;
    p.drawText(m,QString::number(joint->GetReactionForce(1.0/60.0).Length()));
}


//This function check the status of the joint
void Joint::status(){

    if (joint==NULL){ //If the b2Joint is NULL
        emit destroyJoint(this);
        return;
    }
    b2DistanceJoint* joint=(b2DistanceJoint*)this->joint;
    float dx=(joint->GetBodyA()->GetPosition().x-joint->GetBodyB()->GetPosition().x); //Delta x
    float dy=(joint->GetBodyA()->GetPosition().y-joint->GetBodyB()->GetPosition().y); //Delta y
    float l=sqrt(dx*dx+dy*dy); //This is the lenght of the joint
    float force= joint->GetReactionForce(1.0/60.0).Length(); //Get the force applied at the joint

    if (l<50 || l>200 || force>( a->isDragging() || b->isDragging()? 0.6 : 5) ) { //If the joint is too short or too long or the force is too much broke the joint
        a->destroyLink(b);
        b->destroyLink(a);
        emit destroyJoint(this);
    }
}

b2Joint* Joint::getJoint(){
    return joint;
}

bool Joint::has(Goo *a, Goo *b){ //Check if the joint link this two goo
    if (this->a==a && this->b==b) return true;
    if (this->b==a && this->a==b) return true;
    return false;
}

JointType Joint::getType(){
    return type;
}

