#ifndef OG_CONTACTLISTENER_H
#define OG_CONTACTLISTENER_H

#include <Box2D/Box2D.h>

class OGSensor;

class QString;

class OGContactListener : public b2ContactListener
{
    public:
        OGContactListener();
        ~OGContactListener();
        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
        void AddSensor(OGSensor* sensor);
        void RemoveSensor(const QString &id);

    private:
        struct OGContactListenerImpl* pImpl_;
};

#endif // OG_CONTACTLISTENER_H
