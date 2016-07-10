#pragma once

#include <QList>

#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include "og_sensor.h"

namespace og
{
namespace physics
{
class Sensor;

class ContactListener : public b2ContactListener
{
typedef QList<Sensor*> SensorList;

SensorList m_sensors;

public:
    void BeginContact(b2Contact* a_contact);
    void EndContact(b2Contact* a_contact);

    void AddSensor(Sensor* a_sensor)
    {
        m_sensors.push_back(a_sensor);
    }

    void RemoveSensor(Sensor* a_sensor);
};
}
}
