#include "og_forcefield.h"
#include "og_userdata.h"

using namespace physics;

void OGForceField::begin(Fixture* fixture)
{
    b2Body* body = fixture->GetBody();
    void* ud = body->GetUserData();
    OGUserData* data = OGUserData::GetUserData(ud);

    if (data && data->type == OGUserData::BALL) bodies << body;
}

void OGForceField::end(Fixture* fixture)
{
    b2Body* body = fixture->GetBody();
    void* ud = body->GetUserData();
    OGUserData* data = OGUserData::GetUserData(ud);

    if (data && data->type == OGUserData::BALL)
    {
        int i = bodies.indexOf(body);
        bodies.removeAt(i);
    }
}
