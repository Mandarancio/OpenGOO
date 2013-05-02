#ifndef OG_CIRCLESENSOR_H
#define OG_CIRCLESENSOR_H

#include "og_sensor.h"

class Circle;
class OGPCircle;

class OGCircleSensor : public OGSensor
{
    public:
        OGCircleSensor(const QString &id, const Circle &circle);
        ~OGCircleSensor();
        void SetCategory();
        void SetMask();

    private:
        OGPCircle* pBody_;

        Fixture* _GetFixture() const;
        QVector2D _GetPosition() const;

        void _SetCategory(unsigned short category);
        void _SetMask(unsigned short mask);
        void _SetFilter(const OGSensorFilter &filte);
};

#endif // OG_CIRCLESENSOR_H
