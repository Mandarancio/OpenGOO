#pragma once

#include "animationgroup.h"

class SequentialAnimationGroup : public AnimationGroup
{
    void Start(bool aLoop)
    {
        SetLoop(aLoop);

        if (!mAnimation.empty())
        {
            mIsActive = true;
            mCurrentAnimation = 0;
            mAnimation[mCurrentAnimation]->Start();
        }
    }

    void Update(int aDt)
    {
        if (!IsActive())
        {
            return;
        }

        if (mAnimation[mCurrentAnimation]->IsActive())
        {
            mAnimation[mCurrentAnimation]->Update(aDt);
            if (!mAnimation[mCurrentAnimation]->IsActive())
            {
                mCurrentAnimation = (mCurrentAnimation + 1) % mAnimation.size();
                if (mCurrentAnimation == 0)
                {
                    if (GetLoop())
                    {
                        mAnimation[mCurrentAnimation]->Restart();
                    }
                    else
                    {
                        mIsActive = false;
                    }
                }
                else
                {
                    mAnimation[mCurrentAnimation]->Start();
                }
            }
        }
    }

private:
    int mCurrentAnimation;
};
