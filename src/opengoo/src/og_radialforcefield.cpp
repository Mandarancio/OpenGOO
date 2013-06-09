#include "og_forcefield.h"
#include "og_gameengine.h"
#include "opengoo.h"
#include "og_userdata.h"
#include "og_ball.h"

#include <QList>

using namespace physics;

OGRadialForceField::OGRadialForceField(const Circle &c)
    : OGCircleSensor(c), _sqrradius(c.radius() * c.radius())
{
    og::OGSensorFilter f = {0x0004, 0x0010};
    SetFilter(f);
    og::OGPhysicsEngine::GetInstance()->AddSensor(this);
}

void OGRadialForceField::_update()
{   
    int bsize = bodies.size();

    if (!bsize) return;

    b2Vec2 center = pBody_->body->GetPosition();

    for (int i=0; i < bsize; i++)
    {
        b2Body* body = bodies.at(i);
        b2Vec2 position = body->GetPosition();
        b2Vec2 d = center - position;
        float force = _forceatcenter - (d.LengthSquared() * _forceatedge);
        d.Normalize();
        b2Vec2 F = force * d;
        body->ApplyForce(F, position);
    }
}
