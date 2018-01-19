#pragma once

namespace og
{
class IParticleEmiterFactory;

class ParticleEmiterFactoryManager
{
public:
    ParticleEmiterFactoryManager();

    IParticleEmiterFactory* GetFactory() const
    {
        return mFactory;
    }

    void SetFactory(IParticleEmiterFactory* aFactory)
    {
        mFactory = aFactory;
    }

private:
    IParticleEmiterFactory* mFactory;
};
} // ns:og
