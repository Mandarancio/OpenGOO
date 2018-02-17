#pragma once

inline void ReadAttribute(WOGLinearForceField& aData, const QDomAttr& aAttribute)
{
    const auto name = aAttribute.name();
    if (name == QLatin1String("type"))
    {
        OGXmlConfig::WriteValue(aData.type, aAttribute);
    }
    else if (name == QLatin1String("force"))
    {
        OGXmlConfig::WriteValue(aData.force, aAttribute);
    }
    else if (name == QLatin1String("dampeningfactor"))
    {
        OGXmlConfig::WriteValue(aData.dampeningfactor, aAttribute);
    }
    else if (name == QLatin1String("antigrav"))
    {
        OGXmlConfig::WriteValue(aData.antigrav, aAttribute);
    }
    else if (name == QLatin1String("geomonly"))
    {
        OGXmlConfig::WriteValue(aData.geomonly, aAttribute);
    }
    else if (name == QLatin1String("id"))
    {
        OGXmlConfig::WriteValue(aData.id, aAttribute);
    }
    else if (name == QLatin1String("width"))
    {
        OGXmlConfig::WriteValue(aData.width, aAttribute);
    }
    else if (name == QLatin1String("height"))
    {
        OGXmlConfig::WriteValue(aData.height, aAttribute);
    }
    else if (name == QLatin1String("center"))
    {
        OGXmlConfig::WriteValue(aData.center, aAttribute);
    }
    else if (name == QLatin1String("color"))
    {
        OGXmlConfig::WriteValue(aData.color, aAttribute);
    }
    else if (name == QLatin1String("depth"))
    {
        OGXmlConfig::WriteValue(aData.depth, aAttribute);
    }
    else if (name == QLatin1String("enabled"))
    {
        OGXmlConfig::WriteValue(aData.enabled, aAttribute);
    }
    else if (name == QLatin1String("water"))
    {
        OGXmlConfig::WriteValue(aData.water, aAttribute);
    }
}

