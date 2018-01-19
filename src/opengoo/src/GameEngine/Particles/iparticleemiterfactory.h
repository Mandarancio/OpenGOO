#pragma once

#include "Particles/particleemiter.h"

namespace og
{
class IParticleEmiterFactory
{
public:
    virtual ~IParticleEmiterFactory()
    {
    }

    virtual ParticleEmiterSPtr Create(ParticleEmiter::Type aType, int aMaxParticles, ParticleSystem* aSystem) = 0;
};
}

