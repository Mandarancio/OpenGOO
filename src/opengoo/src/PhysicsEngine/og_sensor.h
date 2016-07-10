#pragma once

#include <QVector2D>

#include "og_types.h"


namespace og
{
namespace physics
{
struct SensorFilter
{
    UShort category;
    UShort mask;
};

class Sensor
{
public:
    virtual ~Sensor()
    {
    }

    virtual Fixture* GetFixture() const = 0;

    virtual QVector2D GetPosition() const = 0;

    virtual  void SetCategory(UShort category) = 0;

    virtual  void SetMask(UShort mask) = 0;

    virtual  void SetFilter(const SensorFilter &filter) = 0;

    virtual void BeginContact(Fixture* fixture) = 0;

    virtual void EndContact(Fixture* fixture) = 0;
};
}
}
