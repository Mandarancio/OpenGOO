#ifndef EXITSENSOR_H
#define EXITSENSOR_H

#include <OGCircleSensor>

class ExitSensor : public OGCircleSensor
{
    public:
        ExitSensor(const QString &id, const Circle &circle);
        ~ExitSensor() {}

    private:
        int balls_;

        void _BeginContact(Fixture* fixture);
        void _EndContact(Fixture* fixture);
};

#endif // OG_EXITSENSOR_H
