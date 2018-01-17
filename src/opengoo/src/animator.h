#pragma once

#include <vector>
#include <cmath>

#include "OGLib/util.h"
#include "keyframe.h"

namespace og
{
class Entity;
}

class Animator
{
public:
    enum TransformType
    {
          XFORM_SCALE,
          XFORM_ROTATE,
          XFORM_TRANSLATE
    };

public:
    Animator(og::Entity* aTarget, float aSpeed = 1)
        : mTarget(aTarget)
        , mCurrentTime(0)
        , mCurrentFrameTime(0)
        , mSpeed(std::fabs(aSpeed))
        , mSpeedIsNegative(IsNegative(aSpeed))
    {
    }

    void ProcessRotateTransformFrame();

    void ProcessTransformFrame(TransformType aType)
    {
        switch (aType)
        {
        case XFORM_SCALE:
            break;
        case XFORM_ROTATE:
            ProcessRotateTransformFrame();
            break;
        case XFORM_TRANSLATE:
            break;
        }
    }

    void Update();

    void AddTransformType(TransformType aType)
    {
        mTransformTypes.push_back(aType);
    }

    void AddFrameTime(float aTime)
    {
        mFrameTimes.push_back(aTime);
    }

    void AddTransformFrame(const KeyFrame& aFrame)
    {
        mTransformFrames.push_back(aFrame);
    }

private:
    og::Entity* mTarget;
    std::vector<TransformType> mTransformTypes;
    std::vector<float> mFrameTimes;
    std::vector<KeyFrame> mTransformFrames;
    float mCurrentTime;
    unsigned int mCurrentFrameTime;
    float mSpeed;
    bool mSpeedIsNegative;
};

