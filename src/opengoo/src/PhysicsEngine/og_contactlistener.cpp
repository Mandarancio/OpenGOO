#include "og_contactlistener.h"
#include "og_sensor.h"

#include <QHash>

using namespace og;

typedef QHash<QString, OGSensor*> Hash;
typedef QHashIterator<QString, OGSensor*> HIterator;

struct og::OGContactListenerImpl
{
     Hash sensors;
};

OGContactListener::OGContactListener()
{
    pImpl_ = new OGContactListenerImpl;
}

OGContactListener::~OGContactListener()
{
    delete pImpl_;
}

void OGContactListener::BeginContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();   

    HIterator i(pImpl_->sensors);

    while (i.hasNext())
    {
        i.next();

        b2Fixture* sensor = i.value()->GetFixture();

        if (fixtureA == sensor) i.value()->BeginContact(fixtureB);
        if (fixtureB == sensor) i.value()->BeginContact(fixtureA);
    }
}

void OGContactListener::EndContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    HIterator i(pImpl_->sensors);

    while (i.hasNext())
    {
        i.next();
        b2Fixture* sensor = i.value()->GetFixture();

        if (fixtureA == sensor) i.value()->EndContact(fixtureB);
        if (fixtureB == sensor) i.value()->EndContact(fixtureA);
    }
}

void OGContactListener::AddSensor(OGSensor* sensor)
{
    pImpl_->sensors.insert(sensor->id(), sensor);
}

void OGContactListener::RemoveSensor(const QString &id)
{
    pImpl_->sensors.remove(id);
}

