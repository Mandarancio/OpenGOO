#pragma once

#include "og_config.h"

template<>
struct AttributeReader<OGConfig>
{
    static void Read(const QDomAttr& aAttribute, OGConfig* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("screen_width"))
        {
            ValueWriter::WriteValue(aOut->screen_width, aAttribute);
        }
        else if (name == QLatin1String("screen_height"))
        {
            ValueWriter::WriteValue(aOut->screen_height, aAttribute);
        }
        else if (name == QLatin1String("language"))
        {
            ValueWriter::WriteValue(aOut->language, aAttribute);
        }
        else if (name == QLatin1String("fullscreen"))
        {
            ValueWriter::WriteValue(aOut->fullscreen, aAttribute);
        }
        else if (name == QLatin1String("refreshrate"))
        {
            ValueWriter::WriteValue(aOut->refreshrate, aAttribute);
        }
    }
};
