#ifndef VECTOR2D_H
#define VECTOR2D_H

#include "OGLib/util.h"
#include "pointf.h"

#include <QtGlobal>

namespace opengoo
{
class Vector2D
{
        float x_;
        float y_;

    public:
        Vector2D(float x = 0, float y = 0) : x_(x), y_(y) {}

        Vector2D operator- (Vector2D v)
        {
            return Vector2D(x_ - v.x_, y_ - v.y_);
        }

        Vector2D operator+ (Vector2D v)
        {
            return Vector2D(x_ + v.x_, y_ + v.y_);
        }

        Vector2D operator/ (float n)
        {
            return Vector2D(x_ / n, y_ / n);
        }

        Vector2D operator* (float n)
        {
            return Vector2D(x_ * n, y_ * n);
        }

        void SetZero() { x_ = 0; y_ = 0; }

        Point ToPoint()
        {
            int x = RoundF(x_);
            int y = RoundF(y_);
            return  Point(x, y);
        }
};
}

#endif // VECTOR2D_H
