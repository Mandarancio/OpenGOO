#include "particlesystem.h"
#include "iparticleemmiterfactory.h"

namespace og
{
ParticleSystem::ParticleSystem(const QVector2D &aPosition, float aDepth)
    : Entity(aPosition)
{
    SetDepth(aDepth);
}

ParticleEmmiter *ParticleSystem::CreateEmmiter(ParticleEmmiter::Type aType, int aMaxParticles)
{
    if (auto emmiter = mParticleEmmiterFactoryManager.GetFactory()->Create(aType, aMaxParticles, this))
    {
        emmiter->mIterator = mEmmiters.insert(mEmmiters.end(), emmiter);
        return emmiter.get();
    }

    return nullptr;
}

void ParticleSystem::DestroyEmmiter(ParticleEmmiter* aEmmiter)
{
    mEmmiters.erase(aEmmiter->mIterator);
}

void ParticleSystem::Update()
{
    foreach (auto emmiter, mEmmiters)
    {
        emmiter->Update();
    }
}

void ParticleSystem::Render(QPainter &a_painter)
{
    foreach (auto emmiter, mEmmiters)
    {
        emmiter->Render(a_painter);
    }
}
}
