#pragma once

namespace og
{
class IParticleEmmiterFactory;

class ParticleEmmiterFactoryManager
{
public:
    ParticleEmmiterFactoryManager();

    IParticleEmmiterFactory* GetFactory() const
    {
        return mFactory;
    }

    void SetFactory(IParticleEmmiterFactory* aFactory)
    {
        mFactory = aFactory;
    }

private:
    IParticleEmmiterFactory* mFactory;
};
} // ns:og
