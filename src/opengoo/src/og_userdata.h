#ifndef OG_USERDATA_H
#define OG_USERDATA_H
#include "og_strand.h"

struct OGUserData
{
    enum Type {GEOM, BALL, STRAND};

    Type type;
    void* data;
    int id;
    bool  isTouching;
    bool  isAttachedOnEnter;

    static OGUserData* GetUserData(void* userdata)
    {
        return static_cast<OGUserData*>(userdata);
    }
};

#endif // OG_USERDATA_H
