#pragma once

#include "point.h"

namespace oglib
{
class CircleF
{
    public:
        CircleF() : center_(0, 0), radius_(0) {}

        CircleF(const PointF &center, float radius)
            : center_(center)
            , radius_(radius) {}

        CircleF(float x, float y, float radius)
        {
            CircleF(PointF(x, y), radius);
        }

        PointF center() const { return center_; }

        float x() const { return center_.x(); }
        float y() const { return center_.y(); }

        float radius() const { return radius_; }

        void SetCenter(const PointF &center) { center_ = center; }
        void SetCenter(float x, float y) { center_ = PointF(x, y); }

        void SetX(float x) { center_.SetX(x); }
        void SetY(float y) { center_.SetY(y); }

        void SetRadius(float radius) { radius_ = radius; }

        CircleF operator / (float n)
        {
            return  CircleF(center_ / n, radius_ / n);
        }

    private:
        PointF center_;
        float radius_;
};
}

#endif // CIRCLEF_H
