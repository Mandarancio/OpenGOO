#pragma once

#include <array>

#include "GameEngine/entity.h"

#include "graphicanimation.h"

class ScaleAnimation : public GraphicAnimation
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
        : GraphicAnimation(aDuration)
    {
        mWidthSpeed = (aEndWidth - aStartWidth) / mDuration;
        mHeightSpeed = (aEndHeight - aStartHeight) / mDuration;

        Init(aEntity->GetGraphic());
    }

private:
    void Restart()
    {
        GetGraphic()->SetScaleX(mStartScale[e_width]);
        GetGraphic()->SetScaleY(mStartScale[e_height]);
        Start();
    }

    void Start()
    {
        mScale[e_width] = GetGraphic()->GetScaleX();
        mScale[e_height] = GetGraphic()->GetScaleY();
        mWidth = 0.0f;
        mHeight = 0.0f;
        mRemainingTime = mDuration;
        mIsActive = mRemainingTime > 0;
    }

    void Update(int aDt)
    {
        if (IsActive())
        {
            GetGraphic()->SetScaleX(mScale[e_width] + mWidth);
            GetGraphic()->SetScaleY(mScale[e_height] + mHeight);

            mWidth += mWidthSpeed * aDt;
            mHeight += mHeightSpeed * aDt;

            mRemainingTime -= aDt;
            if (!(mRemainingTime > 0))
            {
                mIsActive = false;
            }
        }
    }

    void Init(og::Graphic* aGraphic)
    {
        SetGraphic(aGraphic);
        mStartScale[e_width] = GetGraphic()->GetScaleX();
        mStartScale[e_height] = GetGraphic()->GetScaleY();
    }

private:
    std::array<float, 2> mStartScale;
    std::array<float, 2> mScale;
    float mWidthSpeed;
    float mHeightSpeed;
    float mWidth;
    float mHeight;
};
