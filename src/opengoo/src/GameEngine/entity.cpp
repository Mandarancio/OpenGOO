#include <QtMath>

#include "scene.h"
#include "entity.h"

namespace og
{
void Entity::DestroyPhysicsBody()
{
    if (mPhysicsBody)
    {
        mPhysicsBody->Destroy();
        mPhysicsBody.reset();
    }
}

void Entity::Render(QPainter& a_painter)
{
    if (m_visible)
    {
        if (mPhysicsBody)
        {
            auto pos = mPhysicsBody->GetPosition() / GetScene()->GetPhysicsEngine()->GetRatio();
            auto a = qRadiansToDegrees(mPhysicsBody->GetAngle());
            m_graphic->SetAngle(-a);
            m_graphic->Render(a_painter, pos.x(), -pos.y());
        }
        else
        {
            m_graphic->Render(a_painter, m_position);
        }
    }
}
}
