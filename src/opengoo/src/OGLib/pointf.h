#ifndef POINTF_H
#define POINTF_H

namespace opengoo
{
class PointF
{
    protected:
        float x_;
        float y_;

    public:
        PointF(float x = 0, float y = 0) : x_(x), y_(y) {}
        virtual ~PointF() {}

        float x() const { return x_; }
        float y() const { return y_; }

        void SetX(float x) { x_ = x; }
        void SetY(float y) { y_ = y; }
};
}

#endif // POINTF_H
