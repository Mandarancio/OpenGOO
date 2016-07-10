#pragma once

#include "og_forcefield.h"
#include "PhysicsEngine/og_circlesensor.h"


class RadialForceField : public ForceField, og::physics::CircleSensor
{
    float m_forceatcenter;
    float m_forceatedge;
    float m_sqrradius;

public:
    RadialForceField(const Circle& c);

    ~RadialForceField()
    {
        PE->RemoveSensor(this);
    }

    void setForceatCenter(float fc)
    {
        m_forceatcenter = fc * -1;
    }

    void setForceatEdge(float fe)
    {
        m_forceatedge = (fe * -1) / m_sqrradius;
    }

private:
    void BeginContact(Fixture* fixture)
    {
        Begin(fixture);
    }

    void EndContact(Fixture* fixture)
    {
        End(fixture);
    }

    void Update();
};
