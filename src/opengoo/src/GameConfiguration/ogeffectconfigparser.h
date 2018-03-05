#pragma once

#include "GameEngine/Particles/particleemiter.h"

#include "enumerator.h"

typedef WOGEffect::WOGParticle::WOGAxialSinOffset AxialSinOffset;

template <> struct Enumerator<AxialSinOffset::Axis>
{
    static AxialSinOffset::Axis ToEnum(const QString& aType)
    {
        return (aType == "y" ? AxialSinOffset::e_y : AxialSinOffset::e_x);
    }
};

template <> struct Enumerator<og::ParticleEmiter::Type>
{
    static og::ParticleEmiter::Type ToEnum(const QString& aType)
    {
        if (aType == "particleeffect")
        {
            return og::ParticleEmiter::e_point;
        }
        else if (aType == "ambientparticleeffect")
        {
            return og::ParticleEmiter::e_ambient;
        }

        return og::ParticleEmiter::e_unknown;
    }
};

struct OGEffectConfigParser
{
    static void WriteAttribute(WOGEffect::WOGParticle& aParticle, const QDomAttr& aAttribute)
    {
        const auto name = aAttribute.name();
        if (name == "scale")
        {
            OGXmlConfig::WriteValue(aParticle.scale, aAttribute);
        }
        else if (name == "finalscale")
        {
            OGXmlConfig::WriteValue(aParticle.finalscale, aAttribute);
        }
        else if(name == "movedirvar")
        {
            OGXmlConfig::WriteValue(aParticle.movedirvar, aAttribute);
        }
        else if (name == "image")
        {
            OGXmlConfig::WriteValue(aParticle.image, aAttribute);
        }
        else if (name == "directed")
        {
            OGXmlConfig::WriteValue(aParticle.directed, aAttribute);
        }
        else if (name == "movedir")
        {
            OGXmlConfig::WriteValue(aParticle.movedir, aAttribute);
        }
        else if (name == "acceleration")
        {
            OGXmlConfig::WriteValue(aParticle.acceleration, aAttribute);
        }
        else if (name == "additive")
        {
            OGXmlConfig::WriteValue(aParticle.additive, aAttribute);
        }
        else if (name == "lifespan")
        {
            OGXmlConfig::WriteValue(aParticle.lifespan, aAttribute);
        }
        else if (name == "speed")
        {
            OGXmlConfig::WriteValue(aParticle.speed, aAttribute);
        }
        else if (name == "dampening")
        {
            OGXmlConfig::WriteValue(aParticle.dampening, aAttribute);
        }
        else if (name == "rotation")
        {
            OGXmlConfig::WriteValue(aParticle.rotation, aAttribute);
        }
        else if (name == "rotspeed")
        {
            OGXmlConfig::WriteValue(aParticle.rotspeed, aAttribute);
        }
        else if (name == "fade")
        {
            OGXmlConfig::WriteValue(aParticle.fade, aAttribute);
        }
    }

    static void WriteAttribute(WOGEffect& aEffect, const QDomAttr& aAttribute)
    {
        const auto name = aAttribute.name();
        if (name == "maxparticles")
        {
            OGXmlConfig::WriteValue(aEffect.maxparticles, aAttribute);
        }
        else if (name == "margin")
        {
            OGXmlConfig::WriteValue(aEffect.margin, aAttribute);
        }
        else if (name == "rate")
        {
            OGXmlConfig::WriteValue(aEffect.rate, aAttribute);
        }
    }

    static void WriteAttribute(AxialSinOffset& aEffect, const QDomAttr& aAttribute)
    {
        const auto name = aAttribute.name();
        if (name == "axis")
        {
            aEffect.axis = Enumerator<AxialSinOffset::Axis>::ToEnum(aAttribute.value());
        }
        else if (name == "freq")
        {
            OGXmlConfig::WriteValue(aEffect.freq, aAttribute);
        }
        else if (name == "amp")
        {
            OGXmlConfig::WriteValue(aEffect.amp, aAttribute);
        }
        else if (name == "phaseshift")
        {
           OGXmlConfig::WriteValue(aEffect.phaseshift, aAttribute);
        }
    }
};

