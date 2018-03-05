#pragma once

#include <memory>

#include <QList>
#include <QPainter>
#include <QVector2D>

namespace og
{
class ImageSource;

class Graphic
{
    Graphic(const Graphic&);
    Graphic& operator=(const Graphic&);

public:
    Graphic()
    {
    }

    virtual ~Graphic()
    {
    }

    virtual void Update() = 0;
    virtual void Render(QPainter& a_painter, const QVector2D& a_pos) = 0;
    virtual void Render(QPainter& a_painter, float aX, float aY) = 0;

    virtual float GetAngle() const = 0;
    virtual void SetAngle(float a_angle)  = 0;

    virtual void SetScaleX(float a_scale) = 0;
    virtual void SetScaleY(float a_scale) = 0;

    virtual float GetScaleX() const = 0;
    virtual float GetScaleY() const = 0;
};
}

typedef std::shared_ptr<og::Graphic> GraphicPtr;
typedef QList<GraphicPtr> GraphicList;
