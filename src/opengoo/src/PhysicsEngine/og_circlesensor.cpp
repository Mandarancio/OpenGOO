#include <QString>

#include "og_circlesensor.h"
#include "circle.h"

#include <OGPhysicsEngine>
#include <OGPCircle>

#include "GameEngine/entity.h"

namespace og
{
namespace physics
{
CircleSensor::CircleSensor(const Circle& a_circle, Entity* a_entity)
    : m_body(PE->CreateCircle(a_circle))
    , m_entity(a_entity)
{
    assert(a_entity);
    m_body->fixture->SetSensor(true);    
}

CircleSensor::~CircleSensor()
{
}

Fixture* CircleSensor::GetFixture() const
{
    return m_body->fixture;
}

QVector2D CircleSensor::GetPosition() const
{
    return m_body->GetPosition();
}

float CircleSensor::GetRadius() const
{
    return m_body->shape->GetRadius();
}

void CircleSensor::SetCategory(UShort category)
{
    auto filter = m_body->fixture->GetFilterData();
    filter.categoryBits = category;
    m_body->fixture->SetFilterData(filter);
}

void CircleSensor::SetMask(UShort mask)
{
    b2Filter filter = m_body->fixture->GetFilterData();
    filter.maskBits = mask;
    m_body->fixture->SetFilterData(filter);
}

void CircleSensor::SetFilter(const SensorFilter& filter)
{
    b2Filter f;
    f.categoryBits = filter.category;
    f.maskBits = filter.mask;
    m_body->fixture->SetFilterData(f);
}

void CircleSensor::BeginContact(Fixture* a_fixture)
{
    m_entity->OnTriggerEnter(a_fixture);
}

void CircleSensor::EndContact(Fixture* a_fixture)
{
    m_entity->OnTriggerExit(a_fixture);
}
}
}
