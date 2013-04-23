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

    static OGUserData* GetUserData(void* userdata)
    {
        OGUserData* data = 0;

        if (userdata) data = static_cast<OGUserData*>(userdata);

        return data;
    }
};

#endif // OG_USERDATA_H
