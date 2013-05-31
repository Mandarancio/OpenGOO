#ifndef EXITSENSOR_H
#define EXITSENSOR_H

#include <OGCircleSensor>

class OGBall;

class ExitSensor : public og::OGCircleSensor
{
    public:
        ExitSensor(const QString &id, const Circle &circle
                   , const og::OGSensorFilter &filter);
        ~ExitSensor() {}

    private:
        int balls_;
        bool isClosed_;

        void _BeginContact(Fixture* fixture);
        void _EndContact(Fixture* fixture);

        void _OpenPipe();
        void _ClosePipe();

        void _ApplyForce(OGBall* ball);
};

#endif // OG_EXITSENSOR_H
