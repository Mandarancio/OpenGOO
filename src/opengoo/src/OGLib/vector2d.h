#pragma once

#include "util.h"
#include "point.h"

#include <QtGlobal>

namespace oglib
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

        PointI ToPoint()
        {
            int x = RoundF(x_);
            int y = RoundF(y_);
            return  PointI(x, y);
        }
};
}
