#pragma once

struct KeyFrame
{
    enum InterpolationType
    {
          INTERPOLATION_NONE,
          INTERPOLATION_LINEAR
    };

    float x;
    float y;
    float angle;
    int alpha;
    int color;
    int nextFrameIndex;
    int soundStrIdx;
    InterpolationType interpolation;
};

