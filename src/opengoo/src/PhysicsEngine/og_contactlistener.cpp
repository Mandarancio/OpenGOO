#include "og_contactlistener.h"

using namespace og;

void OGContactListener::BeginContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();   

    for (int i=0; i < _sensors.size(); i++)
    {
        OGSensor* sensor = _sensors.at(i);
        b2Fixture* fixture = sensor->GetFixture();

        if (fixtureA == fixture)
        {
            sensor->BeginContact(fixtureB);
            break;
        }
        else if (fixtureB == fixture)
        {
            sensor->BeginContact(fixtureA);
            break;
        }
    }
}

void OGContactListener::EndContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    for (int i=0; i < _sensors.size(); i++)
    {
        OGSensor* sensor = _sensors.at(i);
        b2Fixture* fixture = sensor->GetFixture();

        if (fixtureA == fixture)
        {
            sensor->EndContact(fixtureB);
            break;
        }
        else if (fixtureB == fixture)
        {
            sensor->EndContact(fixtureA);
            break;
        }
    }
}
