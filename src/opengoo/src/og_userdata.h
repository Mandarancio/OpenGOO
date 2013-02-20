#ifndef OG_USERDATA_H
#define OG_USERDATA_H
#include "og_strand.h"

struct OGUserData
{
    enum Type {GEOM, BALL, STRAND};

    Type type;
    void* data;
};

#endif // OG_USERDATA_H
