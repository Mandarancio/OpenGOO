#ifndef UTIL_H
#define UTIL_H

inline int RoundF(float d)
{
    return static_cast<int>( d + (d > 0.0 ? 0.5 : -0.5) );
}

#endif // UTIL_H
