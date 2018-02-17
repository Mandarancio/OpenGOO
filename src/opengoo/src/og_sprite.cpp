#include "og_sprite.h"

void OGSprite::Paint(QPainter* aPainter)
{
    if (!mVisible)
    {
        return;
    }

    QPointF target(-mOffsetX, -mOffsetY);
    auto sx = mScaleX;
    auto sy = mScaleY;

    aPainter->save();
    aPainter->translate(GetX(), GetY());

    if (GetAngle() != 0.0f)
    {
        aPainter->rotate(GetAngle());
    }

    aPainter->scale(sx, sy);
    aPainter->setOpacity(mAlpha);
    mSource->Render(*aPainter, target, mClipRect);
    aPainter->restore();
}

void OGSprite::Paint(QPainter& aPainter, const QRectF& aTarget)
{
    if (!mVisible)
    {
        return;
    }

    aPainter.setOpacity(mAlpha);

    aPainter.resetTransform();

    if (GetAngle() != 0.0f)
    {
        aPainter.rotate(GetAngle());
    }

    mSource->Render(aPainter, aTarget, mClipRect);
}

void OGSprite::Render(QPainter& aPainter, const QVector2D& aPosition)
{
    QPointF target(-mOffsetX, -mOffsetY);
    auto sx = mScaleX;
    auto sy = mScaleY;

    aPainter.save();
    aPainter.translate(aPosition.x(), aPosition.y());

    if (GetAngle() != 0.0f)
    {
        aPainter.rotate(GetAngle());
    }

    aPainter.scale(sx, sy);
    aPainter.setOpacity(mAlpha);

    mSource->Render(aPainter, target, mClipRect);

    aPainter.restore();
}

void OGSprite::CenterOrigin()
{
    mOffsetX = mSource->GetWidth() / 2.0f;
    mOffsetY = mSource->GetHeight() / 2.0f;
}
