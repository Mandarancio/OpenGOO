#pragma once

#include <QPainter>
#include <QVector2D>

#include "GameEngine/graphic.h"

class RectSprite : public og::Graphic
{
public:
    RectSprite(const QSizeF& aSize)
        : mRect(QPointF(), aSize)
        , mColor(Qt::green)
    {
        mRect.moveCenter(mRect.topLeft());
    }

private:
    void Update()
    {
    }

    void Render(QPainter& aPainter, const QVector2D& aPos)
    {
        aPainter.save();
        aPainter.setPen(mColor);
        aPainter.translate(aPos.x(), aPos.y());
        aPainter.rotate(mAngle);
        aPainter.drawRect(mRect);
        aPainter.restore();
    }

    float GetAngle() const
    {
        return mAngle;
    }

    void SetAngle(float aAngle)
    {
        mAngle = aAngle;
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
    QRectF mRect;
    QColor mColor;
    float mAngle;
};
