#pragma once

#include "iparticleemiterfactory.h"

namespace og
{
class ParticleSystem;

struct ParticleEmiterFactory : public IParticleEmiterFactory
{
private:
    ParticleEmiterSPtr Create(ParticleEmiter::Type aType, int aMaxParticles, ParticleSystem* aSystem);
};
}
