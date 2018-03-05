#include <QtMath>

#include "og_sprite.h"

#include "GameEngine/scene.h"

#include "strand.h"

void Strand::Render(QPainter& a_painter)
{
    auto spr = static_cast<OGSprite*>(GetGraphic());
    spr->SetScaleY(mLength / spr->GetHeight());
    spr->SetAngle(mAngle);

    Entity::Render(a_painter);
}

void Strand::LastUpdate()
{
    auto p1 = mJoint->GetAnchorA() / GetScene()->GetPhysicsEngine()->GetRatio();
    auto p2 = mJoint->GetAnchorB() / GetScene()->GetPhysicsEngine()->GetRatio();
    SetPosition(p1.x(), -p1.y());

    auto v = p1 - p2;
    mLength = v.length();
    mAngle = qRadiansToDegrees(atan2f(v.x(), v.y()));
}
