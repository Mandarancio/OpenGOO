#pragma once

#include <valarray>
#include <cassert>

#include <QtMath>
#include <QPainter>
#include <QDebug>

#include "../imagesource.h"
#include "../timer.h"

class QPainter;

namespace og
{
class Particle
{
public:
    Particle()
        : mPosition(2)
        , mVelocity(2)
        , mScale(2)
        , mAcceleration(2)
    {
    }

    void Init()
    {
       mShouldRelease = false;
       SetScale(1.0f);
       mFinalScale.first = false;
       mScaleSpeed = 0.0f;
       mShouldUseLifespan = false;
       mFade.first = false;
       mOpacity = 1.0f;
       mIsDirected = false;
       mAngle = 0.0f;
       mRotationSpeed.first = false;
       mIsAdditive = false;
       mDampening.first = false;
    }

    void SetPosition(const QPointF& aPosition)
    {
        mPosition[0] = aPosition.x();
        mPosition[1] = aPosition.y();
    }

    void SetVelocity(float aSpeed, float aDirection)
    {
        const auto rad = qDegreesToRadians(aDirection);
        mVelocity[0] = (std::cos(rad) * aSpeed);
        mVelocity[1] = -(std::sin(rad) * aSpeed);
    }

    void SetImageSource(ImageSourceSPtr aImageSource)
    {
        mImageSource = aImageSource;
        mClipRect = QRectF(0, 0, mImageSource->GetWidth(), mImageSource->GetHeight());
        mTarget = QPointF(-(mImageSource->GetWidth() * 0.5f), -(mImageSource->GetHeight() * 0.5f));
    }

    void SetScale(float aScale)
    {
        mScale[0] = aScale;
        mScale[1] = aScale;
    }

    void SetScaleSpeed(float aSpeed)
    {
        mScaleSpeed = aSpeed;
    }

    void SetFinalScale(float aFinalScale)
    {
        mFinalScale.second = aFinalScale;
    }

    void SetEnabledFinalScale(bool aEnabled)
    {
        mFinalScale.first = aEnabled;
    }

    bool GetEnabledFinalScale() const
    {
        return mFinalScale.first;
    }

    bool GetShouldRelease() const
    {
        return mShouldRelease;
    }

    int GetLifespan() const
    {
        return mLifespanTimer.GetInterval();
    }

    void SetLifespan(int aLifespan)
    {
        mLifespanTimer.SetInterval(aLifespan);
    }

    void SetEnabledLifespan(bool aEnabled)
    {
        mShouldUseLifespan = aEnabled;
    }

    void SetEnabledFade(bool aEnable)
    {
        mFade.first = aEnable;
    }

    void SetFadeSpeed(float aSpeed)
    {
        mFade.second = aSpeed;
    }

    void SetDirected(bool aDirected)
    {
        mIsDirected = aDirected;
    }

    void SetAngle(float aAngle)
    {
        mAngle = aAngle + 180.0f;
    }

    void SetEnabledRotationSpeed(float aEnabled)
    {
        mRotationSpeed.first = aEnabled;
    }

    void SetRotationSpeed(float aSpeed)
    {
        mRotationSpeed.second = aSpeed;
    }

    void SetAcceleration(const QPointF& aAcceleration)
    {
        mAcceleration[0] = aAcceleration.x();
        mAcceleration[1] = -aAcceleration.y();
    }

    void SetEnabledAdditive(bool aEnabled)
    {
        mIsAdditive = aEnabled;
    }

    void SetEnabledDampening(bool aEnabled)
    {
        mDampening.first = aEnabled;
    }

    void SetDampening(float aDampening)
    {
        mDampening.second = aDampening;
    }

    std::valarray<float> GetPosition() const
    {
        return mPosition;
    }

    void SetShouldRelease(bool aShouldRelease)
    {
        mShouldRelease = aShouldRelease;
    }

    void Update()
    {
        mVelocity += mAcceleration;

        if (mDampening.first)
        {
            mVelocity *= 1.0f - mDampening.second;
        }

        mPosition += mVelocity;

        if (GetEnabledFinalScale())
        {
            mScale += mScaleSpeed;
        }

        if (!mIsDirected)
        {
            if (mRotationSpeed.first)
            {
                mAngle += mRotationSpeed.second;
            }
        }
        else
        {
            mAngle = qRadiansToDegrees(std::atan2(mVelocity[0], -mVelocity[1]));
        }

        if (mFade.first)
        {
            mOpacity -= mFade.second;
        }

        if (mShouldUseLifespan)
        {
            mLifespanTimer.Update();

            if (!mLifespanTimer.IsActive())
            {
                mShouldRelease = true;
            }
        }
    }

    void Render(QPainter& aPainter)
    {
        aPainter.save();

        aPainter.translate(mPosition[0], mPosition[1]);
        aPainter.scale(mScale[0], mScale[1]);
        aPainter.setOpacity(mOpacity);

        if (mAngle != 0.0f)
        {
            aPainter.rotate(mAngle);
        }

        if (mIsAdditive)
        {
            aPainter.setCompositionMode(QPainter::CompositionMode_Plus);
        }

        mImageSource->Render(aPainter, mTarget, mClipRect);

        aPainter.restore();
    }

private:
    std::valarray<float> mPosition;
    std::valarray<float> mVelocity;
    std::valarray<float> mScale;
    std::valarray<float> mAcceleration;

    ImageSourceSPtr mImageSource;
    QRectF mClipRect;
    QPointF mTarget;

    float mScaleSpeed;
    std::pair<bool, float> mFinalScale;

    float mAngle;
    std::pair<bool, float> mRotationSpeed;

    std::pair<bool, float> mDampening;

    bool mShouldUseLifespan;
    Timer mLifespanTimer;
    bool mShouldRelease;

    std::pair<bool, float> mFade;
    float mOpacity;

    bool mIsDirected;
    bool mIsAdditive;
};
} // ns:og
