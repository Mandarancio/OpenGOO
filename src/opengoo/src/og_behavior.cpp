#include "og_behavior.h"

void OGBehavior::SetTarget(float posx, float posy)
{
    position = QVector2D(posx, posy);
}

void OGBehavior::SetTarget(const QPointF &pos)
{
    position = QVector2D(pos);
}

void OGBehavior::SetTarget(const QVector2D &pos)
{
    position = pos;
}

void OGBehavior::SetSpeed(float s) { speed = s; }
