#include "exitsensor.h"
#include "circle.h"
#include "opengoo.h"

ExitSensor::ExitSensor(const QString &id, const Circle &circle)
    : OGCircleSensor(id, circle)
    , balls_(0)
{
}

void ExitSensor::_BeginContact(Fixture* fixture)
{
    Q_UNUSED(fixture)

    balls_++;

    if (balls_ == 1) OpenGOO::instance()->OpenPipe();
}

void ExitSensor::_EndContact(Fixture* fixture)
{
    Q_UNUSED(fixture)

    balls_--;

    if (balls_ == 0) OpenGOO::instance()->ClosePipe();
}
