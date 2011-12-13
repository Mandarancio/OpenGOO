#include "fixedgoo.h"
#include "tools.h"

FixedGoo::FixedGoo(b2World *world, QPoint p, int radius, QObject *parent):
    Goo(radius,parent)
{
    b2BodyDef def;
    def.type=b2_staticBody;
    def.position=toVec(p);
    body=world->CreateBody(&def);
    b2CircleShape shape;
    shape.m_p.SetZero();
    shape.m_radius=radius;
    b2FixtureDef fDef;
    fDef.userData=this;
    fDef.density=1.0;
    fDef.shape=&shape;
    body->CreateFixture(&fDef);
    maxJoints=5;
}
