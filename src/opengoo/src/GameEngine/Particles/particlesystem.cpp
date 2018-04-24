#include "particlesystem.h"
#include "iparticleemiterfactory.h"

namespace og
{
ParticleEmiter* ParticleSystem::CreateEmiter(const ParticleEmiterDefination& aDef)
{
    auto emiter = mFactory->Create(aDef, this);
    if (emiter)
    {
        emiter->mIterator = mEmiters.insert(mEmiters.end(), emiter);
    }

    return emiter.get();
}

void ParticleSystem::DestroyEmiter(ParticleEmiter* aEmiter)
{
    mEmiters.erase(aEmiter->mIterator);
}

void ParticleSystem::Update()
{
    foreach (auto emiter, mEmiters)
    {
        emiter->Update();
    }
}

void ParticleSystem::Render(QPainter& aPainter)
{
    foreach (auto emiter, mEmiters)
    {
        emiter->Render(aPainter);
    }
}
}
