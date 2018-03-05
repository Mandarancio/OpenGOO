#pragma once

#include "PhysicsEngine/og_physicsengine.h"

class PhysicsEngineFactory
{
public:
    std::unique_ptr<og::physics::PhysicsEngine> Create()
    {        
        return std::unique_ptr<og::physics::PhysicsEngine>(new og::physics::PhysicsEngine(0, -10, true, 1.0f/32));
    }
};
