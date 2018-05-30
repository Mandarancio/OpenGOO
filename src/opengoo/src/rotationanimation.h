#pragma once

#include "GameEngine/entity.h"

#include "graphicanimation.h"

class RotationAnimation : public GraphicAnimation
{
public:
    RotationAnimation(og::Entity* aEntity, int aDuration, float aStartAngle, float aEndAngle, bool aIsNegative)
        : GraphicAnimation(aDuration)
    {
        mAngleSpeed = aIsNegative ? ((aEndAngle - aStartAngle) / mDuration) : -((aEndAngle - aStartAngle) / mDuration);
        Init(aEntity->GetGraphic());
    }

    RotationAnimation(float aAngleSpeed)
        : GraphicAnimation(-1)
    {
        mAngleSpeed = aAngleSpeed;
    }

private:
    void Restart()
    {
        GetGraphic()->SetAngle(mStartAngle);
        Start();
    }

    void Start()
    {
        mAngle = GetGraphic()->GetAngle();
        mDeltaAngle = 0.0f;
        mRemainingTime = mDuration;
        if (mDuration != -1)
        {
            mIsActive = mRemainingTime > 0;
        }
        else
        {
            mIsActive = true;
        }
    }

    void Update(int aDt)
    {
        if (IsActive())
        {
            GetGraphic()->SetAngle(mAngle + mDeltaAngle);
            mDeltaAngle += mAngleSpeed * aDt;

            if (mRemainingTime != -1)
            {
                mRemainingTime -= aDt;
                if (!(mRemainingTime > 0))
                {
                    mIsActive = false;
                }
            }
        }
    }

    void Init(og::Graphic* aGraphic)
    {
        SetGraphic(aGraphic);
        mStartAngle = GetGraphic()->GetAngle();
    }

private:
    float mStartAngle;
    float mAngleSpeed;
    float mAngle;
    float mDeltaAngle;
};
