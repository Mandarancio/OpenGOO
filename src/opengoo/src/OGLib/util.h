#pragma once

#include <type_traits>

inline int RoundF(float d)
{
    return static_cast<int>( d + (d > 0.0 ? 0.5 : -0.5) );
}

inline bool IsNegative(float val)
{
    union { float f; unsigned int u; } u;
    u.f = val;
    return (int)(u.u >> 31);
}

template<int Min, int Max>
inline bool IsValueInRange(int aVal)
{
    return (aVal >= Min && aVal <= Max);
}

template<typename TOptional, typename TValue>
inline void OptionalSetValue(TOptional& aOptional, const TValue& aValue)
{
    aOptional.first = true;
    aOptional.second = aValue;
}

template<typename TOptional>
inline void OptionalReset(TOptional& aOptional)
{
    aOptional.first = false;
}

template<typename TOptional>
inline bool OptionalHasValue(const TOptional& aOptional)
{
    return aOptional.first;
}

template<typename TOptional>
inline typename std::conditional<
    std::is_const<TOptional>::value, const typename TOptional::second_type, typename TOptional::second_type>::type&
OptionalValue(TOptional& aOptional)
{
    return aOptional.second;
}
