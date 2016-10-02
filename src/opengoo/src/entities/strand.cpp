#include <QtMath>

#include "physics.h"
#include "og_sprite.h"

#include "strand.h"

void Strand::Render(QPainter& a_painter)
{
    auto p1 = m_joint->GetAnchorA() * PhysicsFactory::MetersToPixels;
    auto p2 = m_joint->GetAnchorB() * PhysicsFactory::MetersToPixels;
    auto v = p1 - p2;

    m_sprite->SetScaleY(v.length() / m_sprite->GetHeight());
    m_sprite->SetAngle(qRadiansToDegrees(atan2f(v.x(), v.y())));

    QVector2D p(p1.x(), -p1.y());
    SetPosition(p);

    Entity::Render(a_painter);
}
