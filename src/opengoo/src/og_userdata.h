#pragma once

#include <iostream>

#include "og_strand.h"

struct OGUserData
{
    enum Type
    {
        GEOM,
        BALL,
        STRAND
    };

    Type type;
    void* data;
    int id;
    bool  isTouching;
    bool  isAttachedOnEnter;

    ~OGUserData()
    {
        std::cout << __FUNCTION__ << std::endl;
    }


    static OGUserData* GetUserData(void* userdata)
    {
        return static_cast<OGUserData*>(userdata);
    }

    OGBall* ToBall() const
    {
        return static_cast<OGBall*>(data);
    }

    bool IsBall() const
    {
        return (type == OGUserData::BALL ? true : false);
    }
};
