#pragma once

#include "GameEngine/camera.h"

#include "animation.h"

class CamZoomAnimation : public Animation
{
public:
    CamZoomAnimation(int aDuration, float aStartZoom, float aEndZoom)
        : Animation(aDuration)
    {
        mZoomSpeed = (aEndZoom - aStartZoom) / mDuration;
        mStartZoom = GE->GetCamera()->GetZoom();
    }

private:
    void Restart()
    {
        GE->GetCamera()->SetZoom(mStartZoom);
        Start();
    }

    void Start()
    {
        mCurrentZoom = GE->GetCamera()->GetZoom();
        mDeltaZoom = 0.0f;
        mRemainingTime = mDuration;
        mIsActive = mRemainingTime > 0;
    }

    void Update(int aDt)
    {
        if (IsActive())
        {
            GE->GetCamera()->SetZoom(mCurrentZoom + mDeltaZoom);
            mDeltaZoom += mZoomSpeed * aDt;

            mRemainingTime -= aDt;
            if (!(mRemainingTime > 0))
            {
                mIsActive = false;
            }
        }
    }

private:
    float mStartZoom;
    float mCurrentZoom;
    float mZoomSpeed;
    float mDeltaZoom;
};
