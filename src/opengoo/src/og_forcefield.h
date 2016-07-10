#pragma once

#include <QList>

#include <PhysicsEngine>
#include "og_types.h"

#define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember))


class ForceField
{
    typedef void (ForceField::*ForceFieldMemFn)(b2Body*);

public:
    typedef QList<b2Body*> BodyList;

    virtual ~ForceField()
    {
    }

    virtual void Update()
    {
    }

    void AddBody(b2Body* a_body);
    void RemoveBody(b2Body* a_body);

protected:
     void Begin(Fixture* a_fixture);
     void End(Fixture* a_fixture);

private:
    void ProccessFixture(Fixture* a_fixture, ForceFieldMemFn a_callback);

protected:
    float m_dampeningfactor;
    BodyList m_bodies;
};

class LinearForceField : public ForceField
{
};
