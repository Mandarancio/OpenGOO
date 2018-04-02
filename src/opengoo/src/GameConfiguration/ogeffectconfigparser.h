#pragma once

#include "GameEngine/Particles/particleemiter.h"

#include "Parsers/valuewriter.h"
#include "Parsers/enumerator.h"

//typedef WOGEffect::WOGParticle::WOGAxialSinOffset AxialSinOffset;

//template <> struct Enumerator<AxialSinOffset::Axis>
//{
//    static AxialSinOffset::Axis ToEnum(const QString& aType)
//    {
//        return (aType == "y" ? AxialSinOffset::e_y : AxialSinOffset::e_x);
//    }
//};

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
            ValueWriter::WriteValue(aParticle.scale, aAttribute);
        }
        else if (name == "finalscale")
        {
            ValueWriter::WriteValue(aParticle.finalscale, aAttribute);
        }
        else if(name == "movedirvar")
        {
            ValueWriter::WriteValue(aParticle.movedirvar, aAttribute);
        }
        else if (name == "image")
        {
            ValueWriter::WriteValue(aParticle.image, aAttribute);
        }
        else if (name == "directed")
        {
            ValueWriter::WriteValue(aParticle.directed, aAttribute);
        }
        else if (name == "movedir")
        {
            ValueWriter::WriteValue(aParticle.movedir, aAttribute);
        }
        else if (name == "acceleration")
        {
            ValueWriter::WriteValue(aParticle.acceleration, aAttribute);
        }
        else if (name == "additive")
        {
            ValueWriter::WriteValue(aParticle.additive, aAttribute);
        }
        else if (name == "lifespan")
        {
            ValueWriter::WriteValue(aParticle.lifespan, aAttribute);
        }
        else if (name == "speed")
        {
            ValueWriter::WriteValue(aParticle.speed, aAttribute);
        }
        else if (name == "dampening")
        {
            ValueWriter::WriteValue(aParticle.dampening, aAttribute);
        }
        else if (name == "rotation")
        {
            ValueWriter::WriteValue(aParticle.rotation, aAttribute);
        }
        else if (name == "rotspeed")
        {
            ValueWriter::WriteValue(aParticle.rotspeed, aAttribute);
        }
        else if (name == "fade")
        {
            ValueWriter::WriteValue(aParticle.fade, aAttribute);
        }
    }

    static void WriteAttribute(WOGEffect& aEffect, const QDomAttr& aAttribute)
    {
        const auto name = aAttribute.name();
        if (name == "maxparticles")
        {
            ValueWriter::WriteValue(aEffect.maxparticles, aAttribute);
        }
        else if (name == "margin")
        {
            ValueWriter::WriteValue(aEffect.margin, aAttribute);
        }
        else if (name == "rate")
        {
            ValueWriter::WriteValue(aEffect.rate, aAttribute);
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
            ValueWriter::WriteValue(aEffect.freq, aAttribute);
        }
        else if (name == "amp")
        {
            ValueWriter::WriteValue(aEffect.amp, aAttribute);
        }
        else if (name == "phaseshift")
        {
           ValueWriter::WriteValue(aEffect.phaseshift, aAttribute);
        }
    }
};

