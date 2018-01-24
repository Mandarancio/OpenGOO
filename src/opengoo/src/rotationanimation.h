#pragma once

#include "animation.h"

class RotationAnimation : public Animation
{
public:
    RotationAnimation(og::Entity* aEntity, int aDuration, float aStartAngle, float aEndAngle, bool aIsNegative)
        : Animation(aEntity, aDuration)
    {
        mAngleSpeed = aIsNegative ? ((aEndAngle - aStartAngle) / mDuration) : -((aEndAngle - aStartAngle) / mDuration);
        mStartAngle = mEntity->GetGraphic()->GetAngle();
    }

private:
    void Restart()
    {
        mEntity->GetGraphic()->SetAngle(mStartAngle);
        Start();
    }

    void Start()
    {
        mAngle = mEntity->GetGraphic()->GetAngle();
        mDeltaAngle = 0.0f;
        mRemainingTime = mDuration;
        mIsActive = mRemainingTime > 0;
    }

    void Update(int aDt)
    {
        if (IsActive())
        {
            mEntity->GetGraphic()->SetAngle(mAngle + mDeltaAngle);
            mDeltaAngle += mAngleSpeed * aDt;

            mRemainingTime -= aDt;
            if (!(mRemainingTime > 0))
            {
                mIsActive = false;
            }
        }
    }

private:
    float mStartAngle;
    float mAngleSpeed;
    float mAngle;
    float mDeltaAngle;
};
