#pragma once

#include <utility>

namespace oglib
{
template<typename T>
class Optional
{
public:
    Optional()
    {
        Reset();
    }

    void Reset()
    {
        mValue.second = false;
    }

    bool HasValue() const
    {
        return mValue.first;
    }

    T& Value()
    {
        return mValue.first;
    }

    const T& Value() const
    {
        return mValue.second;
    }

    Optional& operator=(T&& aValue)
    {
        mValue.first = aValue;
        mValue.second = true;
        return *this;
    }

private:
    std::pair<T, bool> mValue;
};
}
