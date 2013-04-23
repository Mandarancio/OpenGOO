#ifndef POINTF_H
#define POINTF_H

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

        PointF operator / (float n)
        {
            return PointF(x_ / n, y_ / n);
        }

    protected:
        float x_;
        float y_;
};
}

#endif // POINTF_H
