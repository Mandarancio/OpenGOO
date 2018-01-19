#include "particlesystem.h"
#include "iparticleemiterfactory.h"

namespace og
{
ParticleSystem::ParticleSystem(const QVector2D &aPosition, float aDepth)
    : Entity(aPosition)
{
    SetDepth(aDepth);
}

ParticleEmiter *ParticleSystem::CreateEmiter(ParticleEmiter::Type aType, int aMaxParticles)
{
    if (auto emiter = mParticleEmiterFactoryManager.GetFactory()->Create(aType, aMaxParticles, this))
    {
        emiter->mIterator = mEmiters.insert(mEmiters.end(), emiter);
        return emiter.get();
    }

    return nullptr;
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
