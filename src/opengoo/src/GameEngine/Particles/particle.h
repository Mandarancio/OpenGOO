#pragma once

#include <array>
#include <valarray>
#include <cassert>

#include <QtMath>
#include <QPainter>
#include <QDebug>

#include "../imagesource.h"
#include "../timer.h"
#include "../../OGLib/util.h"

class QPainter;

namespace og
{
struct HOscillator
{
    float GetAxialSinOffset() const
    {
        return mAxialSinOffset;
    }

    void Init(int aAmplitude, float aPhase)
    {
        mAmplitude = aAmplitude;
        mPhase = aPhase;
        mAngularSpeed = 0.0f;
        mAxialSinOffset = 0.0f;
    }

    void Step(float aAngularSpeedStep)
    {
        mAxialSinOffset = mAmplitude * std::sin(mAngularSpeed + mPhase);
        mAngularSpeed += aAngularSpeedStep;
    }

private:
    int mAmplitude;
    float mPhase;
    float mAngularSpeed;
    float mAxialSinOffset;
};

class Particle
{
public:
    enum Axis
    {
        e_x,
        e_y
    };

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
       OptionalReset(mFinalScale);
       mScaleSpeed = 0.0f;
       mShouldUseLifespan = false;
       OptionalReset(mFade);
       mOpacity = 1.0f;
       mIsDirected = false;
       mAngle = 0.0f;
       OptionalReset(mRotationSpeed);
       mIsAdditive = false;
       OptionalReset(mDampening);

       mShouldUseAxialSinOffset[e_x] = false;
       mShouldUseAxialSinOffset[e_y] = false;
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
        OptionalSetValue(mFinalScale, aFinalScale);
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

    void SetFadeSpeed(float aSpeed)
    {
        OptionalSetValue(mFade, aSpeed);
    }

    void SetDirected(bool aDirected)
    {
        mIsDirected = aDirected;
    }

    void SetAngle(float aAngle)
    {
        mAngle = aAngle + 180.0f;
    }

    void SetRotationSpeed(float aSpeed)
    {
        OptionalSetValue(mRotationSpeed, aSpeed);
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

    void SetDampening(float aDampening)
    {
        OptionalSetValue(mDampening, aDampening);
    }

    std::valarray<float> GetPosition() const
    {
        return mPosition;
    }

    void SetShouldRelease(bool aShouldRelease)
    {
        mShouldRelease = aShouldRelease;
    }

    void SetAxialSinOffset(Axis aAxis, int aAmp, float aAngularSpeedStep, float aPhase)
    {
        switch (aAxis)
        {
        case e_x:
            mHOscillator[e_x].Init(aAmp, aPhase);
            mAngularSpeedStep[e_x] = aAngularSpeedStep;
            mShouldUseAxialSinOffset[e_x] = true;
            break;
        case e_y:
            mHOscillator[e_y].Init(aAmp, aPhase);
            mAngularSpeedStep[e_y] = aAngularSpeedStep;
            mShouldUseAxialSinOffset[e_y] = true;
            break;
        }
    }

    void Update()
    {
        mVelocity += mAcceleration;

        if (OptionalHasValue(mDampening))
        {
            mVelocity *= 1.0f - OptionalValue(mDampening);
        }

        mPosition += mVelocity;

        if (mShouldUseAxialSinOffset[e_x])
        {
            mHOscillator[e_x].Step(mAngularSpeedStep[e_x]);
        }

        if (mShouldUseAxialSinOffset[e_y])
        {
            mHOscillator[e_y].Step(mAngularSpeedStep[e_y]);
        }

        if (OptionalHasValue(mFinalScale))
        {
            mScale += mScaleSpeed;
        }

        if (!mIsDirected)
        {
            if (OptionalHasValue(mRotationSpeed))
            {
                mAngle += OptionalValue(mRotationSpeed);
            }
        }
        else
        {
            mAngle = qRadiansToDegrees(std::atan2(mVelocity[0], -mVelocity[1]));
        }

        if (OptionalHasValue(mFade))
        {
            mOpacity -= OptionalValue(mFade);
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

        auto x = mShouldUseAxialSinOffset[e_x] ?
                    mPosition[0] + mHOscillator[e_x].GetAxialSinOffset() : mPosition[0];
        auto y = mShouldUseAxialSinOffset[e_y] ?
                    mPosition[1] + mHOscillator[e_y].GetAxialSinOffset() : mPosition[1];

        aPainter.translate(x, y);

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

    std::array<bool, 2> mShouldUseAxialSinOffset;
    std::array<HOscillator, 2> mHOscillator;
    std::array<float, 2> mAngularSpeedStep;
};
} // ns:og
