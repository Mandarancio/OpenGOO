#ifndef OG_FORCEFIELD_H
#define OG_FORCEFIELD_H

#include <PhysicsEngine>
#include "PhysicsEngine/og_circlesensor.h"

#include <QList>

namespace physics
{
class OGForceField
{    
public:
    virtual ~OGForceField() {}
    void update() { _update(); }

protected:
    float dampeningfactor;
    QList<b2Body*> bodies;

    void begin(Fixture*);
    void end(Fixture*);

private:    
    virtual void _update() = 0;
};

class OGRadialForceField : public OGForceField, og::OGCircleSensor
{
    float _forceatcenter;
    float _forceatedge;
    float _sqrradius;

public:
    OGRadialForceField(const Circle &c);

    ~OGRadialForceField()
    {
        og::OGPhysicsEngine::GetInstance()->RemoveSensor(this);
    }

    void setForceatCenter(float fc) { _forceatcenter = fc * -1; }
    void setForceatEdge(float fe) { _forceatedge = (fe * -1) / _sqrradius; }

private:
    void _BeginContact(Fixture* fixture) { begin(fixture);}
    void _EndContact(Fixture* fixture) { end(fixture); }
    void _update();
};

class OGLinearForceField : public OGForceField
{
};

} // namespace physics

#endif // OG_FORCEFIELD_H
