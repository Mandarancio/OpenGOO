#include <memory>

#include "particleemiterfactory.h"
#include "pointparticleemiter.h"
#include "ambientparticleemiter.h"

namespace og
{
std::shared_ptr<ParticleEmiter> ParticleEmiterFactory::Create(const ParticleEmiterDefination& aDef,
                                                              ParticleSystem* aSystem)
{
    switch (aDef.type)
    {
    case ParticleEmiter::e_point:
        return std::make_shared<PointParticleEmiter>(aDef.rate, aDef.maxparticles, aSystem);
    case ParticleEmiter::e_ambient:
        return std::make_shared<AmbientParticleEmiter>(aDef.margin, aDef.timeoutInterval, aDef.maxparticles, aSystem);
    case ParticleEmiter::e_unknown:
    case ParticleEmiter::e_user:
        return nullptr;
    }

    return nullptr;
}
}
