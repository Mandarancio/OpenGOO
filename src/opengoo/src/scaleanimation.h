#pragma once

#include <array>

#include "animation.h"

class ScaleAnimation : public Animation
{
    enum
    {
        e_width,
        e_height
    };

public:
    ScaleAnimation(og::Entity* aEntity,
                   int aDuration,
                   float aStartWidth,
                   float aStartHeight,
                   float aEndWidth,
                   float aEndHeight)
        : Animation(aEntity, aDuration)
    {
        mWidthSpeed = (aEndWidth - aStartWidth) / mDuration;
        mHeightSpeed = (aEndHeight - aStartHeight) / mDuration;

        mStartScale[e_width] = mEntity->GetGraphic()->GetScaleX();
        mStartScale[e_height] = mEntity->GetGraphic()->GetScaleY();
    }

private:
    void Restart()
    {
        mEntity->GetGraphic()->SetScaleX(mStartScale[e_width]);
        mEntity->GetGraphic()->SetScaleY(mStartScale[e_height]);
        Start();
    }

    void Start()
    {
        mScale[e_width] = mEntity->GetGraphic()->GetScaleX();
        mScale[e_height] = mEntity->GetGraphic()->GetScaleY();
        mWidth = 0.0f;
        mHeight = 0.0f;
        mRemainingTime = mDuration;
        mIsActive = mRemainingTime > 0;
    }

    void Update(int aDt)
    {
        if (IsActive())
        {
            mEntity->GetGraphic()->SetScaleX(mScale[e_width] + mWidth);
            mEntity->GetGraphic()->SetScaleY(mScale[e_height] + mHeight);

            mWidth += mWidthSpeed * aDt;
            mHeight += mHeightSpeed * aDt;

            mRemainingTime -= aDt;
            if (!(mRemainingTime > 0))
            {
                mIsActive = false;
            }
        }
    }

private:
    std::array<float, 2> mStartScale;
    std::array<float, 2> mScale;
    float mWidthSpeed;
    float mHeightSpeed;
    float mWidth;
    float mHeight;
};
