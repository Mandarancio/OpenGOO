#pragma once

#include "GameEngine/graphic.h"

class CircleSprite : public og::Graphic
{
public:
    CircleSprite(float aRadius, const QColor& aColor = Qt::green)
        : mRadius(aRadius)
        , mColor(aColor)
    {
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

    void Render(QPainter& aPainter, float aX, float aY)
    {
        Render(aPainter, QVector2D(aX, aY));
    }

private:
    float mRadius;
    QColor mColor;
};
