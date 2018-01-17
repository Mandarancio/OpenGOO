#pragma once

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
