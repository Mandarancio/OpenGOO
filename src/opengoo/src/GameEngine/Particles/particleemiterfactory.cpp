#include "particleemiterfactory.h"
#include "pointparticleemiter.h"
#include "ambientparticleemiter.h"

namespace og
{
ParticleEmiterSPtr ParticleEmiterFactory::Create(
        ParticleEmiter::Type aType,
        int aMaxParticles,
        ParticleSystem* aSystem)
{
    switch (aType)
    {
    case ParticleEmiter::e_point:
        return std::make_shared<PointParticleEmiter>(aMaxParticles, aSystem);
    case ParticleEmiter::e_ambient:
        return std::make_shared<AmbientParticleEmiter>(aMaxParticles, aSystem);
    case ParticleEmiter::e_unknown:
    case ParticleEmiter::e_user:
        return nullptr;
    }

    return nullptr;
}
}
