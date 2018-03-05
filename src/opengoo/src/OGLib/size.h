#pragma once

namespace oglib
{
template<typename T>
class Size
{
    T mWidth;
    T mHeight;

public:
    Size(T aWidth=0, T aHeight=0)
        : mWidth(aWidth)
        , mHeight(aHeight)
    {
    }

    T width() const
    {
        return mWidth;
    }

    T height() const
    {
        return mHeight;
    }

    void SetWidth(T aWidth)
    {
        mWidth = aWidth;
    }

    void SetHeight(T aHeight)
    {
        mHeight = aHeight;
    }

    void Set(T aWidth, T aHeight)
    {
        SetWidth(aWidth);
        SetHeight(aHeight);
    }
};

typedef Size<int> SizeI;
typedef Size<float> SizeF;
}
