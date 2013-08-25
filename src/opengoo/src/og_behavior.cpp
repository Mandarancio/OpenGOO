#include "og_behavior.h"

void OGBehavior::SetTarget(float posx, float posy)
{
    if (position!=QVector2D(posx,posy))
    {
        position = QVector2D(posx, posy);
        onTargetChanged();
    }
}

void OGBehavior::SetTarget(const QPointF &pos)
{
    if (position!=QVector2D(pos))
    {
        position = QVector2D(pos);
        onTargetChanged();
    }
}

void OGBehavior::SetTarget(const QVector2D &pos)
{
    if (position!=pos)
    {
        position = pos;
        onTargetChanged();
    }
}

void OGBehavior::SetSpeed(float s) { speed = s; }

void OGIClimbBehavior::SetOrigin(float posx, float posy)
{
    if (origin!=QVector2D(posx,posy))
    {
        origin = QVector2D(posx, posy);
        onOriginChanged();
    }
}

void OGIClimbBehavior::SetOrigin(const QPointF &pos)
{
    if (origin!=QVector2D(pos))
    {
        origin = QVector2D(pos);
        onOriginChanged();
    }
}

void OGIClimbBehavior::SetOrigin(const QVector2D &pos)
{
    if (origin!=pos)
    {
        origin = pos;
        onOriginChanged();
    }
}
