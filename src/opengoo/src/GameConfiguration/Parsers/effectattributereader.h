#pragma once

#include "wog_effects.h"

typedef WOGEffect::WOGParticle::WOGAxialSinOffset AxialSinOffset;

template <>
struct Enumerator<AxialSinOffset::Axis>
{
    static AxialSinOffset::Axis ToEnum(const QString& aType)
    {
        return (aType == "y" ? AxialSinOffset::e_y : AxialSinOffset::e_x);
    }
};

template<>
struct AttributeReader<WOGEffects>
{
    void Read(const QDomAttr& aAttribute, WOGEffect* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("maxparticles"))
        {
            ValueWriter::WriteValue(aOut->maxparticles, aAttribute);
        }
        else if (name == QLatin1String("rate"))
        {
            ValueWriter::WriteValue(aOut->rate, aAttribute);
        }
        else if (name == QLatin1String("margin"))
        {
            ValueWriter::WriteValue(aOut->margin, aAttribute);
        }
    }

    void Read(const QDomAttr& aAttribute, WOGEffect::WOGParticle* aOut)
    {
        const auto name = aAttribute.name();
        if (name == "scale")
        {
            ValueWriter::WriteValue(aOut->scale, aAttribute);
        }
        else if (name == "finalscale")
        {
            ValueWriter::WriteValue(aOut->finalscale, aAttribute);
        }
        else if(name == "movedirvar")
        {
            ValueWriter::WriteValue(aOut->movedirvar, aAttribute);
        }
        else if (name == "image")
        {
            ValueWriter::WriteValue(aOut->image, aAttribute);
        }
        else if (name == "directed")
        {
            ValueWriter::WriteValue(aOut->directed, aAttribute);
        }
        else if (name == "movedir")
        {
            ValueWriter::WriteValue(aOut->movedir, aAttribute);
        }
        else if (name == "acceleration")
        {
            ValueWriter::WriteValue(aOut->acceleration, aAttribute);
        }
        else if (name == "additive")
        {
            ValueWriter::WriteValue(aOut->additive, aAttribute);
        }
        else if (name == "lifespan")
        {
            ValueWriter::WriteValue(aOut->lifespan, aAttribute);
        }
        else if (name == "speed")
        {
            ValueWriter::WriteValue(aOut->speed, aAttribute);
        }
        else if (name == "dampening")
        {
            ValueWriter::WriteValue(aOut->dampening, aAttribute);
        }
        else if (name == "rotation")
        {
            ValueWriter::WriteValue(aOut->rotation, aAttribute);
        }
        else if (name == "rotspeed")
        {
            ValueWriter::WriteValue(aOut->rotspeed, aAttribute);
        }
        else if (name == "fade")
        {
            ValueWriter::WriteValue(aOut->fade, aAttribute);
        }
    }

    void Read(const QDomAttr& aAttribute, WOGEffect::WOGParticle::WOGAxialSinOffset* aOut)
    {
        const auto name = aAttribute.name();
        if (name == "axis")
        {
            aOut->axis = Enumerator<AxialSinOffset::Axis>::ToEnum(aAttribute.value());
        }
        else if (name == "freq")
        {
            ValueWriter::WriteValue(aOut->freq, aAttribute);
        }
        else if (name == "amp")
        {
            ValueWriter::WriteValue(aOut->amp, aAttribute);
        }
        else if (name == "phaseshift")
        {
           ValueWriter::WriteValue(aOut->phaseshift, aAttribute);
        }
    }
};
