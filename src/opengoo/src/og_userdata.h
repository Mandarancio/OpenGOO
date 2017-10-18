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

    OGUserData()
    {
    }

    OGUserData(Type a_Type, void* a_Data)
        : type(a_Type)
        , data(a_Data)
    {
    }

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

    class Builder
    {
        Type m_type;
        void* m_data;

    public:
        Builder& SetType(Type a_type)
        {
            m_type = a_type;
            return *this;
        }

        Builder& SetData(void* a_data)
        {
            m_data = a_data;
            return *this;
        }

        std::unique_ptr<OGUserData> Build()
        {
            return std::unique_ptr<OGUserData>(new OGUserData(m_type, m_data));
        }
    };
};
