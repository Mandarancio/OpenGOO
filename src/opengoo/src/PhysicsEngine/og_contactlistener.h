#ifndef OG_CONTACTLISTENER_H
#define OG_CONTACTLISTENER_H

#include <Box2D/Box2D.h>

#include "og_sensor.h"

#include <QList>

namespace og
{
class OGSensor;

class OGContactListener : public b2ContactListener
{
    public:
        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
        void AddSensor(OGSensor* sensor) { _sensors << sensor; }
        void RemoveSensor(OGSensor* sensor);

    private:                        
        QList<OGSensor*> _sensors;
};

inline void OGContactListener::RemoveSensor(OGSensor* sensor)
{
    int i = _sensors.indexOf(sensor);
    if (i != -1) _sensors.removeAt(i);
}

} // namespace og

#endif // OG_CONTACTLISTENER_H
