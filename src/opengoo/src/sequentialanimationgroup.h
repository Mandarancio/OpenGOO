#pragma once

#include "animationgroup.h"

class SequentialAnimationGroup : public AnimationGroup
{
    void Start()
    {
        if (!mAnimation.empty())
        {
            mCurrentAnimation = 0;
            mAnimation[mCurrentAnimation]->Start();
        }
    }

    void Update(int aDt)
    {
        if (!mAnimation.empty())
        {
            if (mAnimation[mCurrentAnimation]->IsActive())
            {
                mAnimation[mCurrentAnimation]->Update(aDt);
                if (!mAnimation[mCurrentAnimation]->IsActive())
                {
                    mCurrentAnimation = (mCurrentAnimation + 1) % mAnimation.size();
                    if (mCurrentAnimation == 0)
                    {
                        mAnimation[mCurrentAnimation]->Restart();
                    }
                    else
                    {
                        mAnimation[mCurrentAnimation]->Start();
                    }
                }
            }
        }
    }

private:
    int mCurrentAnimation;
};
