#pragma once

#include <vector>
#include <string>

struct AnimationData
{
    enum TransformType
    {
          XFORM_SCALE = 0,
          XFORM_ROTATE = 1,
          XFORM_TRANSLATE = 2
    };

    enum InterpolationType
    {
          INTERPOLATION_NONE = 0,
          INTERPOLATION_LINEAR = 1
    };

    struct KeyFrame
    {
        float x;
        float y;
        float angle;
        int alpha;
        int color;
        int nextFrameIndex;
        int soundStrIdx;
        InterpolationType interpolation;
    };

    struct TransformEntry
    {
        TransformType type;
        std::vector<KeyFrame> frame;
    };

    std::vector<float> frameTime;
    std::vector<TransformEntry> transformFrame;
    std::vector<KeyFrame> alphFrame;
    std::vector<KeyFrame> colorFrame;
    std::vector<KeyFrame> soundFrame;
    std::string stringTable;
};
