#pragma once

#include <cassert>
#include <QDebug>

namespace og
{
struct Timer
{
    Timer()
        : mInterval(0)
        , mRemainingTime(-1)
    {
    }

    void SetInterval(int aInterval)
    {
        assert(aInterval >= 0 && "SetInterval. aInterval is negative");

        mInterval = aInterval;
        Start();
    }

    int GetInterval() const
    {
        return mInterval;
    }

    void Start()
    {
        mRemainingTime = mInterval;
    }

    int IsActive() const
    {
        return mRemainingTime != -1;
    }

    void Update()
    {
        if (mRemainingTime != -1)
        {
            --mRemainingTime;
        }
    }

    int mInterval;
    int mRemainingTime;
};
}
