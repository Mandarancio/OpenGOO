#include "particleemmiterfactorymanager.h"
#include "particleemmiterfactory.h"

namespace og
{
static ParticleEmmiterFactory DefaultParticleEmmiterFactory;

ParticleEmmiterFactoryManager::ParticleEmmiterFactoryManager()
    : mFactory(&DefaultParticleEmmiterFactory)
{
}
}
