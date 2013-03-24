#ifndef POINT_H
#define POINT_H

namespace opengoo
{
class Point
{
    protected:
        int x_;
        int y_;

    public:
        Point(int x = 0, int y = 0) : x_(x), y_(y) {}
        virtual ~Point() {}

        int x() const { return x_; }
        int y() const { return y_; }

        void SetX(int x) { x_ = x; }
        void SetY(int y) { y_ = y; }

};
}

#endif // POINT_H
