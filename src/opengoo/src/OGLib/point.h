#pragma once

namespace oglib
{
template<typename T>
class Point
{
    public:
        Point(T x = 0, T y = 0)
            : x_(x)
            , y_(y)
        {
        }

        virtual ~Point()
        {
        }

        T x() const
        {
            return x_;
        }

        T y() const
        {
            return y_;
        }

        void SetX(T x)
        {
            x_ = x;
        }

        void SetY(T y)
        {
            y_ = y;
        }

        void Set(T x, T y)
        {
            x_ = x;
            y_ = y;
        }

        Point<T> operator / (float n)
        {
            return Point<T>(x_ / n, y_ / n);
        }

        Point<T> operator - (const Point<T>& aVal)
        {
            return Point<T>(x() - aVal.x(), y() - aVal.y());
        }

        Point<T>& operator *= (float aVal)
        {
            x_ *= aVal;
            y_ *= aVal;
            return *this;
        }

    protected:
        T x_;
        T y_;
};

typedef Point<int> PointI;
typedef Point<float> PointF;
}
