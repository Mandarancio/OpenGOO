#pragma once

#include "iparticleemiterfactory.h"

namespace og
{
struct ParticleEmiterFactory : public IParticleEmiterFactory
{
private:
    std::shared_ptr<ParticleEmiter> Create(const ParticleEmiterDefination& aDef, ParticleSystem* aSystem);
};
}
