#pragma once

#include "wog_ball.h"
#include "og_xmlconfig.h"
#include "enumerator.h"

template <> struct Enumerator<WOGBallShape::Type>
{
    static WOGBallShape::Type ToEnum(const QString& aType)
    {
        if (aType == QLatin1String("circle"))
        {
            return WOGBallShape::e_circle;
        }
        if (aType == QLatin1String("rectangle"))
        {
            return WOGBallShape::e_rectangle;
        }

        return WOGBallShape::e_unknown;
    }
};

inline void WriteValue(WOGBallShape& aData, const QDomAttr& aAttribute)
{
    auto values = aAttribute.value().split(',');
    if (values.size() == 0)
    {
        return;
    }

    aData.type = Enumerator<WOGBallShape::Type>::ToEnum(values[0]);
    if (aData.type == WOGBallShape::e_circle)
    {
        for (int i = 1; i < values.size(); ++i)
        {
            switch (i)
            {
            case 1:
                OGXmlConfig::WriteValue(aData.radius, values[1]);
                aData.radius *= 0.5f;
                break;
            case 2:
                OGXmlConfig::WriteValue(aData.variation, values[2]);
                break;
            }
        }
    }
    else if (aData.type == WOGBallShape::e_rectangle)
    {
        for (int i = 1; i < values.size(); ++i)
        {
            switch (i)
            {
            case 1:
                OGXmlConfig::WriteValue(aData.width, values[1]);
                break;
            case 2:
                OGXmlConfig::WriteValue(aData.height, values[2]);
                break;
            case 3:
                OGXmlConfig::WriteValue(aData.variation, values[3]);
                break;
            }
        }
    }
}

struct AttributeReader
{
    static void Read(const QDomAttr& aAttribute, WOGPart* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("name"))
        {
            OGXmlConfig::WriteValue(aOut->name, aAttribute);
        }
        else if (name == QLatin1String("image"))
        {
            OGXmlConfig::WriteValue(aOut->image, aAttribute);
        }
        else if (name == QLatin1String("layer"))
        {
            OGXmlConfig::WriteValue(aOut->layer, aAttribute);
        }
        else if (name == QLatin1String("scale"))
        {
            OGXmlConfig::WriteValue(aOut->scale, aAttribute);
        }
        else if (name == QLatin1String("rotate"))
        {
            OGXmlConfig::WriteValue(aOut->rotate, aAttribute);
        }
        else if (name == QLatin1String("stretch"))
        {
            aOut->stretch = aAttribute.value();
        }
        else if (name == QLatin1String("state"))
        {
            OGXmlConfig::WriteValue(aOut->state, aAttribute);
        }
        else if (name == QLatin1String("x"))
        {
            OGXmlConfig::WriteValue(aOut->x, aAttribute);
        }
        else if (name == QLatin1String("y"))
        {
            OGXmlConfig::WriteValue(aOut->y, aAttribute);
        }
        else if (name == QLatin1String("xrange"))
        {
            OGXmlConfig::WriteValue(aOut->xrange, aAttribute);
        }
        else if (name == QLatin1String("yrange"))
        {
            OGXmlConfig::WriteValue(aOut->yrange, aAttribute);
        }
        else if (name == QLatin1String("eye"))
        {
            OGXmlConfig::WriteValue(aOut->eye, aAttribute);
        }
        else if (name == QLatin1String("pupilinset"))
        {
            OGXmlConfig::WriteValue(aOut->pupilinset, aAttribute);
        }
        else if (name == QLatin1String("pupil"))
        {
            OGXmlConfig::WriteValue(aOut->pupil, aAttribute);
        }
    }

    static void Read(const QDomAttr& aAttribute, WOGBallMarker* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("drag"))
        {
            OGXmlConfig::WriteValue(aOut->drag, aAttribute);
        }
        else if (name == QLatin1String("detach"))
        {
            OGXmlConfig::WriteValue(aOut->detach, aAttribute);
        }
        else if (name == QLatin1String("rotspeed"))
        {
            OGXmlConfig::WriteValue(aOut->rotspeed, aAttribute);
        }
    }

    static void Read(const QDomAttr& aAttribute, WOGBallCoreAttributes* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("name"))
        {
            OGXmlConfig::WriteValue(aOut->name, aAttribute);
        }
        else if (name == QLatin1String("shape"))
        {
            ::WriteValue(aOut->shape, aAttribute);
        }
        else if (name == QLatin1String("mass"))
        {
            OGXmlConfig::WriteValue(aOut->mass, aAttribute);
        }
        else if (name == QLatin1String("strands"))
        {
            OGXmlConfig::WriteValue(aOut->strands, aAttribute);
        }
        else if (name == QLatin1String("material"))
        {
            OGXmlConfig::WriteValue(aOut->material, aAttribute);
        }
        else if (name == QLatin1String("towermass"))
        {
            OGXmlConfig::WriteValue(aOut->towermass, aAttribute);
        }
        else if (name == QLatin1String("dragmass"))
        {
            OGXmlConfig::WriteValue(aOut->dragmass, aAttribute);
        }
    }

    static void Read(const QDomAttr& /*aAttribute*/, WOGBallBehaviourAttributes* /*aOut*/)
    {
    }

    static void Read(const QDomAttr& aAttribute, WOGBallAttributes* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("spawn"))
        {
            OGXmlConfig::WriteValue(aOut->spawn, aAttribute);
        }
        else
        {
            Read(aAttribute, &aOut->core);
            Read(aAttribute, &aOut->behaviour);
        }
    }
};
