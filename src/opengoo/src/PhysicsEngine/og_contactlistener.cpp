#include "og_contactlistener.h"

namespace og
{
namespace physics
{
void ContactListener::BeginContact(b2Contact* a_contact)
{
    auto fixtureA = a_contact->GetFixtureA();
    auto fixtureB = a_contact->GetFixtureB();
    Fixture* fixture;
    Sensor* sensor;

    for (int i = 0; i < m_sensors.size(); ++i)
    {
        sensor = m_sensors[i];
        fixture = sensor->GetFixture();

        if (fixtureA == fixture)
        {
            sensor->BeginContact(fixtureB);
            break;
        }

        if (fixtureB == fixture)
        {
            sensor->BeginContact(fixtureA);
            break;
        }
    }
}

void ContactListener::EndContact(b2Contact* a_contact)
{
    auto fixtureA = a_contact->GetFixtureA();
    auto fixtureB = a_contact->GetFixtureB();
    b2Fixture* fixture;
    Sensor* sensor;

    for (int i = 0; i < m_sensors.size(); ++i)
    {
        sensor = m_sensors[i];
        fixture = sensor->GetFixture();

        if (fixtureA == fixture)
        {
            sensor->EndContact(fixtureB);
            break;
        }

        if (fixtureB == fixture)
        {
            sensor->EndContact(fixtureA);
            break;
        }
    }
}

void ContactListener::RemoveSensor(Sensor* a_sensor)
{
    int i = m_sensors.indexOf(a_sensor);

    if (i != -1)
        m_sensors.removeAt(i);
}
}
}
