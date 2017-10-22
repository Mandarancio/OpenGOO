#pragma once

namespace oglib
{
class Size
{
    int mWidth;
    int mHeight;

public:
    Size(int aWidth=0, int aHeight=0)
        : mWidth(aWidth)
        , mHeight(aHeight)
    {
    }

    int width() const
    {
        return mWidth;
    }

    int height() const
    {
        return mHeight;
    }

    void SetWidth(int aWidth)
    {
        mWidth = aWidth;
    }

    void SetHeight(int aHeight)
    {
        mHeight = aHeight;
    }

    void Set(int aWidth, int aHeight)
    {
        SetWidth(aWidth);
        SetHeight(aHeight);
    }
};
}
