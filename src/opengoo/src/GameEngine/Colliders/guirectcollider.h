#pragma once

#include "rectcollider.h"

namespace og
{
class GUIRectCollider : public og::RectCollider
{
public:
    GUIRectCollider(float aWidth, float aHeight)
        : RectCollider(aWidth, aHeight)
    {
    }

    GUIRectCollider(float aWidth, float aHeight, float aCenterX, float aCenterY, float aAngle)
        : RectCollider(aWidth, aHeight, aCenterX, aCenterY, aAngle)
    {
    }

private:
    bool OverlapPoint(const QVector2D&) const;
};
}

