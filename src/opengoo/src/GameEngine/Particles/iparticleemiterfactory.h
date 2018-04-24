#pragma once

namespace og
{
class ParticleSystem;
class ParticleEmiter;
struct ParticleEmiterDefination;

struct IParticleEmiterFactory
{
    virtual ~IParticleEmiterFactory()
    {
    }

    virtual std::shared_ptr<ParticleEmiter> Create(const ParticleEmiterDefination& aDef, ParticleSystem* aSystem) = 0;
};
}

