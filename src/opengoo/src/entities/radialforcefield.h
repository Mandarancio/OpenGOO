#pragma once

#include "GameEngine/entity.h"
#include "PhysicsEngine/og_circlesensor.h"

class OGBall;

class RadialForceField : public og::Entity
{
public:
    typedef QList<OGBall*> BallList;

    class Builder;

public:
    RadialForceField(
            og::physics::PhysicsEngine& a_physicEngine,
            const Circle& a_c,
            float a_forceatcenter,
            float a_forceatedge,
            const og::physics::SensorFilter& a_filter);

private:
    void OnTriggerEnter(Fixture* a_fixture);

    void OnTriggerExit(Fixture* a_fixture);

    void Added();

    void Removed();

    void Update();

    void Render(QPainter& a_painter);

private:
    og::physics::CircleSensor m_sensor;
    float m_forceatcenter;
    float m_forceatedge;
    BallList m_balls;
    BallList m_remove;
    og::physics::PhysicsEngine& m_physicEngine;
};

class RadialForceField::Builder
{
    QPointF m_center;
    float m_radius;
    float m_forceatcenter;
    float m_forceatedge;
    og::physics::SensorFilter m_filter;
    og::physics::PhysicsEngine& m_physicEngine;

public:
    Builder(og::physics::PhysicsEngine& a_physicEngine)
        : m_radius(0.0f)
        , m_forceatcenter(0.0f)
        , m_forceatedge(0.0f)
        , m_physicEngine(a_physicEngine)
    {
        m_filter.category = 0;
        m_filter.mask = 0;
    }

    Builder& SetCenter(const QPointF& a_center)
    {
        m_center = a_center;
        return *this;
    }

    Builder& SetRadius(float a_radius)
    {
        m_radius = a_radius;
        return *this;
    }

    Builder& SetForceAtCenter(float a_forceatcenter)
    {
        m_forceatcenter = a_forceatcenter;
        return *this;
    }

    Builder& SetForceAtEdge(float a_forceatedge)
    {
        m_forceatedge = a_forceatedge;
        return *this;
    }

    Builder& SetFilter(og::physics::SensorFilter& a_filter)
    {
        m_filter = a_filter;
        return *this;
    }

    Builder& SetCategory(UShort a_category)
    {
        m_filter.category = a_category;
        return *this;
    }

    Builder& SetMask(UShort a_mask)
    {
        m_filter.mask = a_mask;
        return *this;
    }

    std::shared_ptr<RadialForceField> Build()
    {
        return std::make_shared<RadialForceField>(m_physicEngine, Circle(m_center, m_radius), m_forceatcenter, m_forceatedge, m_filter);
    }
};
