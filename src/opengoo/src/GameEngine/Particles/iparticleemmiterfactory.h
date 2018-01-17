#pragma once

#include "Particles/particleemmiter.h"

namespace og
{
class IParticleEmmiterFactory
{
public:
    virtual ~IParticleEmmiterFactory()
    {
    }

    virtual ParticleEmmiterSPtr Create(ParticleEmmiter::Type aType, int aMaxParticles, ParticleSystem* aSystem) = 0;
};
}

