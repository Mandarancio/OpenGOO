#pragma once

#include <QLineF>

#include "GameEngine/graphic.h"

class LineSprite : public og::Graphic
{
public:
    LineSprite(const QLineF& aLine, const QColor& aColor = Qt::green)
        : mLine(aLine)
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

    void Render(QPainter& aPainter, float /*aX*/, float /*aY*/)
    {
        aPainter.save();
        aPainter.setPen(mColor);
        aPainter.drawLine(mLine);
        aPainter.restore();
    }

    void Render(QPainter& aPainter, const QVector2D& aPos)
    {
        Render(aPainter, aPos.x(), aPos.y());
    }

private:
    QLineF mLine;
    QColor mColor;
};
