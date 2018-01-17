#pragma once

#include "iparticleemmiterfactory.h"

namespace og
{
class ParticleSystem;

struct ParticleEmmiterFactory : public IParticleEmmiterFactory
{
private:
    ParticleEmmiterSPtr Create(ParticleEmmiter::Type aType, int aMaxParticles, ParticleSystem* aSystem);
};
}
