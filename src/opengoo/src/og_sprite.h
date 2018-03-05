#pragma once

#include <memory>

#include <QPainter>
#include <QVector2D>

#include "wog_vobject.h"

#include "GameEngine/imagesource.h"
#include "GameEngine/graphic.h"

class QPainter;

class OGSprite;

typedef std::shared_ptr<OGSprite> OGSpritePtr;

class OGSprite : public og::Graphic
{    
    QVector2D mPosition;
    og::ImageSourceSPtr mSource;
    float mAngle;
    float mOffsetX;
    float mOffsetY;
    bool mVisible;
    float mScaleX;
    float mScaleY;
    float mAlpha;
    float mDepth;
    QRectF mClipRect;
    QColor mColorize;

    void Init()
    {
        mVisible = true;
        mAngle = 0.0f;
        mOffsetX = 0.0f;
        mOffsetY = 0.0f;
        mScaleX = 1.0f;
        mScaleY = 1.0f;
        mAlpha = 1.0f;
        mDepth = 0.0f;
        mClipRect = QRectF(0, 0, GetWidth(), GetHeight());
    }

public:
    OGSprite()
    {
        Init();
    }

    OGSprite(og::ImageSourceSPtr aSource)
        : mSource(aSource)
    {
        Init();
    }

    OGSprite(float aX, float aY, og::ImageSourceSPtr aSource)
        : mPosition(aX, aY)
        , mSource(aSource)
    {
        Init();
    }

    void Paint(QPainter* aPainter);

    void Paint(QPainter& aPainter, const QRectF& aTarget);

    void Render(QPainter& aPainter, float aX, float aY);

    void Render(QPainter& aPainter, const QVector2D& aPosition)
    {
        Render(aPainter, aPosition.x(), aPosition.y());
    }

    void Update()
    {
    }

    void SetAngle(float aAngle)
    {
        mAngle = aAngle;
    }

    float GetAngle() const
    {
        return mAngle;
    }

    void CenterOrigin();

    float GetX() const
    {
        return mPosition.x();
    }

    float GetY() const
    {
        return mPosition.y();
    }

    void SetX(float aX)
    {
        mPosition.setX(aX);
    }

    void SetY(float aY)
    {
        mPosition.setY(aY);
    }

    void SetVisible(bool aVisible)
    {
        mVisible = aVisible;
    }

    void SetPosition(float aX, float aY)
    {
        mPosition.setX(aX);
        mPosition.setY(aY);
    }

    const QVector2D& GetPosition() const
    {
        return mPosition;
    }

    int GetWidth() const
    {
        return mSource->GetWidth();
    }

    int GetHeight() const
    {
        return mSource->GetHeight();
    }

    void SetScaleX(float aScale)
    {
        mScaleX = aScale;
    }

    void SetScaleY(float aScale)
    {
        mScaleY = aScale;
    }

    void SetScale(const QPointF& aScale)
    {
        SetScaleX(aScale.x());
        SetScaleY(aScale.y());
    }

    float GetScaleX() const
    {
        return mScaleX;
    }

    float GetScaleY() const
    {
        return mScaleY;
    }

    float GetScaledWidth() const
    {
        return GetWidth() * mScaleX;
    }

    float GetScaledHeight() const
    {
        return GetHeight() * mScaleY;
    }

    void SetAlpha(float aAlpha)
    {
        mAlpha = aAlpha;
    }

    float GetAlpha() const
    {
        return mAlpha;
    }

    void SetDepth(float aDepth)
    {
        mDepth = aDepth;
    }

    float GetDepth() const
    {
        return mDepth;
    }

    void SetOffsetX(float aOffset)
    {
        mOffsetX = aOffset;
    }

    void SetOffsetY(float aOffset)
    {
        mOffsetY = aOffset;
    }

    void SetColorize(const QColor& aColor)
    {
       mColorize = aColor;
    }

    og::ImageSource* GetImageSource() const
    {
        return mSource.get();
    }

    static OGSpritePtr Create(og::ImageSourceSPtr aSource)
    {
        return std::make_shared<OGSprite>(aSource);
    }
};
