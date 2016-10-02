#pragma once

#include <QVector2D>

namespace og {
class Collider
{
public:
    virtual ~Collider()
    {
    }

    virtual bool OverlapPoint(const QVector2D& a_point) const = 0;
};
}
