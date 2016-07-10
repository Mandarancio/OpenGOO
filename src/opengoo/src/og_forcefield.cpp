#include <functional>

#include "og_forcefield.h"
#include "og_userdata.h"


void ForceField::AddBody(b2Body* a_body)
{
    assert(a_body != nullptr);
    assert(m_bodies.indexOf(a_body) == -1);
    m_bodies.push_back(a_body);
}

void ForceField::RemoveBody(b2Body* a_body)
{
    assert(a_body != nullptr);
    int i = m_bodies.indexOf(a_body);
    assert(m_bodies.indexOf(a_body) != -1);
    m_bodies.removeAt(i);
}

void ForceField::ProccessFixture(Fixture* a_fixture, ForceFieldMemFn a_callback)
{
    assert(a_fixture != nullptr);
    auto body = a_fixture->GetBody();
    auto ud = body->GetUserData();

    if (!ud)
        return;

    auto data = static_cast<OGUserData*>(ud);

    if (data->IsBall())
        CALL_MEMBER_FN(*this, a_callback)(body);
}

void ForceField::Begin(Fixture* a_fixture)
{
    assert(a_fixture != nullptr);
    ProccessFixture(a_fixture, &ForceField::AddBody);
}

void ForceField::End(Fixture* a_fixture)
{
    assert(a_fixture != nullptr);
    ProccessFixture(a_fixture, &ForceField::RemoveBody);
}
