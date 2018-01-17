#include "particleemmiterfactory.h"
#include "pointparticleemmiter.h"
#include "ambientparticleemmiter.h"

namespace og
{
ParticleEmmiterSPtr ParticleEmmiterFactory::Create(
        ParticleEmmiter::Type aType,
        int aMaxParticles,
        ParticleSystem* aSystem)
{
    switch (aType)
    {
    case ParticleEmmiter::e_point:
        return std::make_shared<PointParticleEmmiter>(aMaxParticles, aSystem);
    case ParticleEmmiter::e_ambient:
        return std::make_shared<AmbientParticleEmmiter>(aMaxParticles, aSystem);
    case ParticleEmmiter::e_unknown:
    case ParticleEmmiter::e_user:
        return nullptr;
    }

    return nullptr;
}
}
