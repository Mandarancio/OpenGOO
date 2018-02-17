#pragma once

namespace oglib
{
class PointF
{
    public:
        PointF(float x = 0, float y = 0) : x_(x), y_(y) {}
        virtual ~PointF() {}

        float x() const { return x_; }
        float y() const { return y_; }

        void SetX(float x) { x_ = x; }
        void SetY(float y) { y_ = y; }
        void Set(float x, float y)
        {
            SetX(x);
            SetY(y);
        }

        PointF operator / (float n)
        {
            return PointF(x_ / n, y_ / n);
        }

        PointF operator - (const PointF& aVal)
        {
            return PointF(x() - aVal.x(), y() - aVal.y());
        }

    protected:
        float x_;
        float y_;
};
}
