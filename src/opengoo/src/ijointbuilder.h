#pragma once

#include "PhysicsEngine/joint.h"

struct IJointBuilder
{
    typedef std::unique_ptr<og::physics::Joint> JointUPtr;

    virtual ~IJointBuilder()
    {
    }

    virtual JointUPtr Build() = 0;
};
