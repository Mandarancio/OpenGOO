#include "joint.h"
#include "tools.h"

#include "publicclass.h"
#include <QLinearGradient>

Joint::Joint(Goo *a, Goo *b, b2World *world,bool child, QObject *parent):
    QObject(parent)
{

    this->a=a;
    this->b=b;
    type=NORMAL;
    if (!child) initialize(world);
    animationValue=0.0;
    counter=1;

    sSystem = SoundSystem::GetInstance();
    if (sSystem->IsFail()) { popSound = SoundSystem::NONETYPE; }
    else { popSound = sSystem->Create(SoundSystem::POP); }
}

Joint::~Joint()
{
    sSystem->Delete(popSound);
}

void Joint::initialize(b2World * world){
    b2DistanceJointDef jDef;
    jDef.Initialize(a->getBody(),b->getBody(),a->getVPosition(),b->getVPosition());
    jDef.dampingRatio=0.4;
    jDef.frequencyHz=3;
    jDef.collideConnected=true;
    joint=(b2DistanceJoint*)world->CreateJoint(&jDef);
}

void Joint::paint(QPainter &p){
    QLinearGradient lg(this->a->getPPosition(),this->b->getPPosition());
    lg.setColorAt(0,this->a->getColor());
    lg.setColorAt(1,this->b->getColor());
    QPen pen;
    pen.setBrush(lg);
    pen.setWidth(4);
    p.setPen(pen);
    drawLines(p);

    if (counter>0){
        counter++;
    }
    else if (counter<0){
        counter--;
    }
    if ((qAbs(counter)==1) || !qAbs((counter)%3)){
        animationValue+=0.06*(counter/qAbs(counter));
        if (animationValue>=1.0){
            counter=-1;
            animationValue=1;
        }
        else if (animationValue<=0.0){
            animationValue=0;
            counter=+1;
        }
    }

    QColor anColor(255,255,255);
    if (a->getColor().blackF()<0.5 && b->getColor().blackF()<0.5)
        anColor.setAlpha(30);

    else
        anColor.setAlpha(8);

    lg=QLinearGradient(a->getPPosition(),b->getPPosition());
    lg.setColorAt(0,Qt::transparent);
    lg.setColorAt(1,Qt::transparent);
    lg.setColorAt(animationValue,anColor);


    pen.setBrush(lg);
    p.setPen(pen);
    drawLines(p);


}

void Joint::paintDebug(QPainter &p){
    p.setPen(Qt::green  );
    p.drawLine(toPoint(joint->GetBodyA()->GetPosition()),toPoint(joint->GetBodyB()->GetPosition()));
    QPoint m=(toPoint(joint->GetBodyA()->GetPosition())+toPoint(joint->GetBodyB()->GetPosition()))/2;
    p.drawText(m,QString::number(joint->GetReactionForce(realStep/4.0).Length()));
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
    float force= joint->GetReactionForce(realStep/4.0).Length(); //Get the force applied at the joint

    if (l<5.0 || l>20.0 || force>( a->isDragging() || b->isDragging()? 0.01 : 8.0) ) { //If the joint is too short or too long or the force is too much broke the joint
        a->destroyLink(b);
        b->destroyLink(a);
        if (a->isDragging() || b->isDragging()){
            Goo* d=(a->isDragging()? a : b);

           // soundSystem.setPitch(source.first,float(d->getRadius())/24.0);
            sSystem->SetVolume(popSound, 0.2*float(d->getRadius())/24.0);
            sSystem->SetPosition(popSound, d->getPPosition());
            sSystem->Play(popSound);

        }
        emit destroyJoint(this);
    }
}

void Joint::drawLines(QPainter &p){
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

bool Joint::has(Goo *a, Goo *b){ //Check if the joint link this two goo
    if (this->a==a && this->b==b) return true;
    if (this->b==a && this->a==b) return true;
    return false;
}

Goo* Joint::goo(bool id){
    if (id) return a;
    else return b;
}

QRect Joint::boundingRect(){
    QRect rect=a->boundingRect();
    rect=rect.united(b->boundingRect());
    return rect;
}

QPolygon Joint::boundingPoly(){
    QPolygon poly(5);
    QRect ra,rb;
    ra=a->boundingRect();
    rb=b->boundingRect();

    poly.setPoint(0,ra.topRight());
    poly.setPoint(1,ra.bottomLeft());
    poly.setPoint(2,rb.bottomLeft());
    poly.setPoint(3,rb.topRight());
    poly.setPoint(4,ra.topRight());

    return poly;
}

