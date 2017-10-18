#pragma once

#include <memory>

#include "og_sensor.h"
#include "og_pcircle.h"


class Circle;

namespace og
{
class Entity;

namespace physics
{
class PhysicsEngine;

class CircleSensor : public Sensor
{
    typedef std::unique_ptr<OGPCircle> OGPCirclePtr;

    OGPCirclePtr m_body;
    Entity* m_entity;

public:
    CircleSensor(og::physics::PhysicsEngine& a_physicEngine, const Circle& a_circle, Entity* a_entity);
    ~CircleSensor();

    Fixture* GetFixture() const;
    QVector2D GetPosition() const;
    float GetRadius() const;

    void SetCategory(UShort category);
    void SetMask(UShort mask);
    void SetFilter(const SensorFilter& filte);

private:
    void BeginContact(Fixture* a_fixture);

    void EndContact(Fixture* a_fixture);
};
}
}
