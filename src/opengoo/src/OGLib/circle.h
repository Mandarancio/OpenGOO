#ifndef CIRCLE_H
#define CIRCLE_H

#include "point.h"

namespace oglib
{
class Circle
{
    public:
        Circle() : center_(0, 0), radius_(0) {}

        Circle(const Point &center, int radius)
            : center_(center)
            , radius_(radius) {}

        Circle(int x, int y, int radius)
        {
            Circle(Point(x, y), radius);
        }

        Point center() const { return center_; }

        int x() const { return center_.x(); }
        int y() const { return center_.y(); }

        int radius() const { return radius_; }

        void SetPosition(const Point &center) { center_ = center; }
        void SetPosition(int x, int y) { center_ = Point(x, y); }

        void SetX(int x) { center_.SetX(x); }
        void SetY(int y) { center_.SetY(y); }

        void SetRadius(int radius) { radius_ = radius; }

    private:
        Point center_;
        int radius_;
};
}

#endif // CIRCLE_H
