#pragma once

#include "Parsers/valuewriter.h"

inline void ReadAttribute(WOGLinearForceField& aData, const QDomAttr& aAttribute)
{
    const auto name = aAttribute.name();
    if (name == QLatin1String("type"))
    {
        ValueWriter::WriteValue(aData.type, aAttribute);
    }
    else if (name == QLatin1String("force"))
    {
        ValueWriter::WriteValue(aData.force, aAttribute);
    }
    else if (name == QLatin1String("dampeningfactor"))
    {
        ValueWriter::WriteValue(aData.dampeningfactor, aAttribute);
    }
    else if (name == QLatin1String("antigrav"))
    {
        ValueWriter::WriteValue(aData.antigrav, aAttribute);
    }
    else if (name == QLatin1String("geomonly"))
    {
        ValueWriter::WriteValue(aData.geomonly, aAttribute);
    }
    else if (name == QLatin1String("id"))
    {
        ValueWriter::WriteValue(aData.id, aAttribute);
    }
    else if (name == QLatin1String("width"))
    {
        ValueWriter::WriteValue(aData.width, aAttribute);
    }
    else if (name == QLatin1String("height"))
    {
        ValueWriter::WriteValue(aData.height, aAttribute);
    }
    else if (name == QLatin1String("center"))
    {
        ValueWriter::WriteValue(aData.center, aAttribute);
    }
    else if (name == QLatin1String("color"))
    {
        ValueWriter::WriteValue(aData.color, aAttribute);
    }
    else if (name == QLatin1String("depth"))
    {
        ValueWriter::WriteValue(aData.depth, aAttribute);
    }
    else if (name == QLatin1String("enabled"))
    {
        ValueWriter::WriteValue(aData.enabled, aAttribute);
    }
    else if (name == QLatin1String("water"))
    {
        ValueWriter::WriteValue(aData.water, aAttribute);
    }
}

