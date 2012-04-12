#include "balloongoo.h"
#include "tools.h"

BalloonGoo::BalloonGoo(b2World *world, QPoint p, int radius, QObject *parent):
    Goo(radius,parent)
{

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
    active=false;
    maxJoints=7; //parameters
}

void BalloonGoo::paint(QPainter &p){
    p.setPen(Qt::darkGray);
    p.setBrush(Qt::yellow);
    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
}

void BalloonGoo::paintDebug(QPainter &p){
    p.setPen(Qt::darkGray);
    p.setBrush(Qt::transparent);

    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());

}

void BalloonGoo::contactGround(){
 //TODO
}
void BalloonGoo::contactGround(QPoint p){
    if (p.isNull()) return;
 //TODO
}
