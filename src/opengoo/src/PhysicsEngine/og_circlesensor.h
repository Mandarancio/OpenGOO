#ifndef OG_CIRCLESENSOR_H
#define OG_CIRCLESENSOR_H

#include "og_sensor.h"
#include "og_pcircle.h"

class Circle;

namespace og
{
class OGCircleSensor : public og::OGSensor
{
    public:
        OGCircleSensor(const Circle &circle);
        ~OGCircleSensor();

    protected:
        OGPCircle* pBody_;

    private:
        Fixture* _GetFixture() const;
        QVector2D _GetPosition() const;

        void _SetCategory(unsigned short category);
        void _SetMask(unsigned short mask);
        void _SetFilter(const OGSensorFilter &filte);
};
}

#endif // OG_CIRCLESENSOR_H
