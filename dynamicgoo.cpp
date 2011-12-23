#include "dynamicgoo.h"
#include "tools.h"

DynamicGoo::DynamicGoo(b2World *world, QPoint p, int radius,  QObject *parent):
    Goo(radius,parent)
{
    b2BodyDef def;
    def.awake=true;
    def.type=b2_dynamicBody;
    def.position=toVec(p);
    body=world->CreateBody(&def);
    b2CircleShape shape;
    shape.m_p.SetZero();
    shape.m_radius=radius;
    b2FixtureDef fixDef;
    fixDef.restitution=0.3;
    fixDef.density=1.0;
    fixDef.friction=0.8;
    fixDef.shape=&shape;
    fixDef.userData=this;
    body->CreateFixture(&fixDef);
    body->SetLinearDamping(0.8);//Not sure about this parameter
    moovable=true;
    dragable=true;
    maxJoints=7;
}
