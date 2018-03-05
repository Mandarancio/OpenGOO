#pragma once

#include "og_physicsshape.h"

namespace og
{
namespace physics
{
struct FixtureBuilder
{
    FixtureBuilder(b2Body* aBody)
        : mBody(aBody)
    {
    }

    FixtureBuilder& SetShape(const b2Shape* aShape)
    {
        mFixtureDef.shape = aShape;
        return *this;
    }

    FixtureBuilder& SetFriction(float aFriction)
    {
        mFixtureDef.friction = aFriction;
        return *this;
    }

    FixtureBuilder& SetDensity(float aDensity)
    {
        mFixtureDef.density = aDensity;
        return *this;
    }

    FixtureBuilder& SetRestitution(float aRestitution)
    {
        mFixtureDef.restitution = aRestitution;
        return *this;
    }

    FixtureBuilder& SetSensor(bool aSensor)
    {
        mFixtureDef.isSensor = aSensor;
        return *this;
    }

    FixtureBuilder& SetFilter(const b2Filter& aFilter)
    {
        mFixtureDef.filter = aFilter;
        return *this;
    }

    FixtureBuilder& SetUserData(void* aUserData)
    {
        mFixtureDef.userData = aUserData;
        return *this;
    }

    b2Fixture* Build()
    {
        if (!mFixtureDef.shape)
        {
            return nullptr;
        }

        return mBody->CreateFixture(&mFixtureDef);
    }

private:
    b2FixtureDef mFixtureDef;
    b2Body* mBody;
};
}
}
