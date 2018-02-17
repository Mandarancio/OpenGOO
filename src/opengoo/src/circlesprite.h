#pragma once

#include <QPainter>
#include <QVector2D>

#include "GameEngine/graphic.h"

class CircleSprite : public og::Graphic
{
public:
    CircleSprite(float aRadius)
        : mRadius(aRadius)
        , mColor(Qt::green)
    {
    }

private:
    void Update()
    {
    }

    void Render(QPainter& aPainter, const QVector2D& aPos)
    {
        aPainter.save();
        aPainter.setPen(mColor);
        aPainter.drawEllipse(aPos.toPointF(), mRadius, mRadius);
        aPainter.restore();
    }

    float GetAngle() const
    {
        return 0;
    }

    void SetAngle(float)
    {
    }

    void SetScaleX(float)
    {
    }

    void SetScaleY(float)
    {
    }

    float GetScaleX() const
    {
        return 1;
    }

    float GetScaleY() const
    {
        return 1;
    }

private:
    float mRadius;
    QColor mColor;
};
