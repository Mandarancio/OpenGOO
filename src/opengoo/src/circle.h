#pragma once

#include <QVector2D>

class Circle
{
public:
    Circle(float x, float y, float radius)
        : center_(x, y), radius_(radius) {}
    Circle(const QVector2D &center, float radius)
        : center_(center), radius_(radius) {}
    Circle(const QPointF &center, float radius)
        : center_(center), radius_(radius) {}

    QVector2D center() const { return center_; }
    float radius() const { return radius_; }

    Circle operator / (float divisor)
    {
        return  Circle(center_ / divisor, radius_ / divisor);
    }

    friend const Circle operator * (const Circle& lhs, float factor)
    {
        return Circle(lhs.center_ * factor, lhs.radius_ * factor);
    }

private:
    QVector2D center_;
    float radius_;
};
