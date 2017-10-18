#include "PhysicsEngine/og_physicsengine.h"
#include "physics.h"
#include "og_userdata.h"

#include "radialforcefield.h"

RadialForceField::RadialForceField(
    og::physics::PhysicsEngine& a_physicEngine,
    const Circle& a_c,
    float a_forceatcenter,
    float a_forceatedge,
    const og::physics::SensorFilter& a_filter)
    : Entity(a_c.center().x(), a_c.center().y())
    , m_sensor(a_physicEngine, a_c * PhysicsFactory::PixelsToMeters, this)
    , m_forceatcenter(a_forceatcenter)
    , m_forceatedge(a_forceatedge)
    , m_physicEngine(a_physicEngine)
{
    m_sensor.SetFilter(a_filter);
}

void RadialForceField::Added()
{
   m_physicEngine.AddSensor(&m_sensor);
}

void RadialForceField::Removed()
{
    m_physicEngine.RemoveSensor(&m_sensor);
}

void RadialForceField::OnTriggerEnter(Fixture* a_fixture)
{
    auto userdata = a_fixture->GetBody()->GetUserData();
    if (!userdata)
        return;

    auto data = static_cast<OGUserData*>(userdata);
    if (data->IsBall())
    {
        auto b = data->ToBall();
        assert(m_balls.indexOf(b) == -1);
        m_balls.push_back(b);
    }
}

void RadialForceField::OnTriggerExit(Fixture* a_fixture)
{
    auto userdata = a_fixture->GetBody()->GetUserData();
    if (!userdata)
        return;

    auto data = static_cast<OGUserData*>(userdata);
    if (data->IsBall())
    {
        auto b = data->ToBall();
        m_remove.push_back(b);
    }
}

void RadialForceField::Render(QPainter& a_painter)
{
    a_painter.save();
    a_painter.setPen(Qt::red);
    auto c = m_sensor.GetPosition() * PhysicsFactory::MetersToPixels;
    auto r = m_sensor.GetRadius() * PhysicsFactory::MetersToPixels;
    a_painter.drawEllipse(c.toPointF(), r, r);
    a_painter.restore();
}

void RadialForceField::Update()
{
    QVector2D center = m_sensor.GetPosition();

    for (int i = 0; i < m_balls.size(); ++i)
    {
        QVector2D position = m_balls[i]->GetPhyPosition();
        QVector2D v = center - position;

        if (v.length() < FLT_EPSILON)
            continue;

        float force = m_forceatcenter - v.length() * m_forceatedge;

        v.normalize();
        v *= force;
        m_balls[i]->GetBody()->ApplyForce(v, position);
    }

    if (!m_remove.isEmpty())
    {
        for (int i = 0; i < m_remove.size(); ++i)
        {
            m_balls.removeOne(m_remove[i]);
        }

        m_remove.clear();
    }
}
