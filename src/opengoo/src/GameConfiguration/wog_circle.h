#pragma once

#include "wog_pobject.h"
#include <wogimage.h>

struct WOGCircle : public WOGPhysicsObject
{
    float radius;
    float rotspeed;
    WOGImage image;

    WOGCircle()
        : radius(0)
        , rotspeed(0)
    {
    }

    bool HasImage() const
    {
        return !image.IsEmpty();
    }
};
