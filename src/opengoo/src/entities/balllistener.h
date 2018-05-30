#pragma once

class Ball;

struct BallListener
{
    virtual ~BallListener()
    {
    }

    virtual void OnMarker(Ball* aBall) = 0;

    virtual void OnPickUp() = 0;
};
