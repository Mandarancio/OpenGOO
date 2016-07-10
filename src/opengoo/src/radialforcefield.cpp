#include <QList>

#include "og_forcefield.h"
#include "og_gameengine.h"
#include "opengoo.h"
#include "og_userdata.h"
#include "entities/og_ball.h"


RadialForceField::RadialForceField(const Circle& c)
    : CircleSensor(c, nullptr), m_sqrradius(c.radius() * c.radius())
{
    og::physics::SensorFilter f;
    f.category = 0x0004;
    f.mask = 0x0010;
    SetFilter(f);
    PE->AddSensor(this);
}

void RadialForceField::Update()
{   
    int bsize = m_bodies.size();

    if (!bsize)
        return;

    auto pos = GetPosition();
    b2Vec2 center(pos.x(), pos.y());

    for (int i = 0; i < bsize; ++i)
    {
        b2Body* body = m_bodies[i];
        b2Vec2 position = body->GetPosition();
        b2Vec2 d = center - position;
        float force = m_forceatcenter - (d.LengthSquared() * m_forceatedge);
        d.Normalize();
        b2Vec2 F = force * d;
        body->ApplyForce(F, position);
    }
}
