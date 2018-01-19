#include "particleemiterfactorymanager.h"
#include "particleemiterfactory.h"

namespace og
{
static ParticleEmiterFactory DefaultParticleEmiterFactory;

ParticleEmiterFactoryManager::ParticleEmiterFactoryManager()
    : mFactory(&DefaultParticleEmiterFactory)
{
}
}
