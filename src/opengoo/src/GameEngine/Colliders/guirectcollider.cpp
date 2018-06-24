#include "GameEngine/input.h"

#include "guirectcollider.h"

namespace og
{
bool GUIRectCollider::OverlapPoint(const QVector2D&) const
{
    QVector2D p(og::MouseInput::GetPosition());
    return RectCollider::OverlapPoint(p);
}
}
