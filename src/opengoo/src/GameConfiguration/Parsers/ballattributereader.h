#pragma once

#include "wog_ball.h"

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
                ValueWriter::WriteValue(aData.radius, values[1]);
                aData.radius *= 0.5f;
                break;
            case 2:
                ValueWriter::WriteValue(aData.variation, values[2]);
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
                ValueWriter::WriteValue(aData.width, values[1]);
                break;
            case 2:
                ValueWriter::WriteValue(aData.height, values[2]);
                break;
            case 3:
                ValueWriter::WriteValue(aData.variation, values[3]);
                break;
            }
        }
    }
}

template<>
struct AttributeReader<WOGBall>
{
    static void Read(const QDomAttr& aAttribute, WOGPart* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("name"))
        {
            ValueWriter::WriteValue(aOut->name, aAttribute);
        }
        else if (name == QLatin1String("image"))
        {
            ValueWriter::WriteValue(aOut->image, aAttribute);
        }
        else if (name == QLatin1String("layer"))
        {
            ValueWriter::WriteValue(aOut->layer, aAttribute);
        }
        else if (name == QLatin1String("scale"))
        {
            ValueWriter::WriteValue(aOut->scale, aAttribute);
        }
        else if (name == QLatin1String("rotate"))
        {
            ValueWriter::WriteValue(aOut->rotate, aAttribute);
        }
        else if (name == QLatin1String("stretch"))
        {
            aOut->stretch = aAttribute.value();
        }
        else if (name == QLatin1String("state"))
        {
            ValueWriter::WriteValue(aOut->state, aAttribute);
        }
        else if (name == QLatin1String("x"))
        {
            ValueWriter::WriteValue(aOut->x, aAttribute);
        }
        else if (name == QLatin1String("y"))
        {
            ValueWriter::WriteValue(aOut->y, aAttribute);
        }
        else if (name == QLatin1String("xrange"))
        {
            ValueWriter::WriteValue(aOut->xrange, aAttribute);
        }
        else if (name == QLatin1String("yrange"))
        {
            ValueWriter::WriteValue(aOut->yrange, aAttribute);
        }
        else if (name == QLatin1String("eye"))
        {
            ValueWriter::WriteValue(aOut->eye, aAttribute);
        }
        else if (name == QLatin1String("pupilinset"))
        {
            ValueWriter::WriteValue(aOut->pupilinset, aAttribute);
        }
        else if (name == QLatin1String("pupil"))
        {
            ValueWriter::WriteValue(aOut->pupil, aAttribute);
        }
    }

    static void Read(const QDomAttr& aAttribute, WOGBallMarker* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("drag"))
        {
            ValueWriter::WriteValue(aOut->drag, aAttribute);
        }
        else if (name == QLatin1String("detach"))
        {
            ValueWriter::WriteValue(aOut->detach, aAttribute);
        }
        else if (name == QLatin1String("rotspeed"))
        {
            ValueWriter::WriteValue(aOut->rotspeed, aAttribute);
        }
    }

    static void Read(const QDomAttr& aAttribute, WOGBallCoreAttributes* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("name"))
        {
            ValueWriter::WriteValue(aOut->name, aAttribute);
        }
        else if (name == QLatin1String("shape"))
        {
            ::WriteValue(aOut->shape, aAttribute);
        }
        else if (name == QLatin1String("mass"))
        {
            ValueWriter::WriteValue(aOut->mass, aAttribute);
        }
        else if (name == QLatin1String("strands"))
        {
            ValueWriter::WriteValue(aOut->strands, aAttribute);
        }
        else if (name == QLatin1String("material"))
        {
            ValueWriter::WriteValue(aOut->material, aAttribute);
        }
        else if (name == QLatin1String("towermass"))
        {
            ValueWriter::WriteValue(aOut->towermass, aAttribute);
        }
        else if (name == QLatin1String("dragmass"))
        {
            ValueWriter::WriteValue(aOut->dragmass, aAttribute);
        }
    }

    static void Read(const QDomAttr& /*aAttribute*/, WOGBallBehaviourAttributes* /*aOut*/)
    {
    }

    static void Read(const QDomAttr& aAttribute, WOGBallMovementAttributes* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("walkspeed"))
        {
            ValueWriter::WriteValue(aOut->walkspeed, aAttribute);
        }
        else if (name == QLatin1String("climbspeed"))
        {
            ValueWriter::WriteValue(aOut->climbspeed, aAttribute);
        }
        else if (name == QLatin1String("speedvariance"))
        {
            ValueWriter::WriteValue(aOut->speedvariance, aAttribute);
        }
        else if (name == QLatin1String("dampening"))
        {
            ValueWriter::WriteValue(aOut->dampening, aAttribute);
        }
        else if (name == QLatin1String("walkforce"))
        {
            ValueWriter::WriteValue(aOut->walkforce, aAttribute);
        }
        else if (name == QLatin1String("thrust"))
        {
            ValueWriter::WriteValue(aOut->thrust, aAttribute);
        }
    }

    static void Read(const QDomAttr& aAttribute, WOGBallPlayerInteraction* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("draggable"))
        {
            ValueWriter::WriteValue(aOut->draggable, aAttribute);
        }
        else if (name == QLatin1String("detachable"))
        {
            ValueWriter::WriteValue(aOut->detachable, aAttribute);
        }
        else if (name == QLatin1String("hingedrag"))
        {
            ValueWriter::WriteValue(aOut->hingedrag, aAttribute);
        }
        else if (name == QLatin1String("fling"))
        {
            ValueWriter::WriteValue(aOut->fling, aAttribute);
        }
    }

    static void Read(const QDomAttr& aAttribute, WOGBallAttributes* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("spawn"))
        {
            ValueWriter::WriteValue(aOut->spawn, aAttribute);
        }
        else
        {
            Read(aAttribute, &aOut->core);
            Read(aAttribute, &aOut->behaviour);
            Read(aAttribute, &aOut->movement);
            Read(aAttribute, &aOut->player);
        }
    }
};
