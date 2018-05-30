#pragma once

#include "wog_level.h"

template <> struct Enumerator<WOGCamera::Aspect>
{
    static WOGCamera::Aspect ToEnum(const QString& aType)
    {
        if (aType == QLatin1String("normal"))
        {
            return WOGCamera::Normal;
        }
        if (aType == QLatin1String("widescreen"))
        {
            return WOGCamera::WideScreen;
        }

        return WOGCamera::Unknown;
    }
};

template<>
struct AttributeReader<WOGPoi>
{
    static void Read(const QDomAttr& aAttribute, WOGPoi* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("pos"))
        {
            ValueWriter::WriteValue(aOut->position, aAttribute);
        }
        else if (name == QLatin1String("traveltime"))
        {
            ValueWriter::WriteValue(aOut->traveltime, aAttribute);
        }
        else if (name == QLatin1String("pause"))
        {
            ValueWriter::WriteValue(aOut->pause, aAttribute);
        }
        else if (name == QLatin1String("zoom"))
        {
            ValueWriter::WriteValue(aOut->zoom, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGCamera>
{
    static void Read(const QDomAttr& aAttribute, WOGCamera* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("aspect"))
        {
            aOut->aspect = Enumerator<WOGCamera::Aspect>::ToEnum(aAttribute.value());
        }
        else if (name == QLatin1String("endpos"))
        {
            ValueWriter::WriteValue(aOut->endpos, aAttribute);
        }
        else if (name == QLatin1String("endzoom"))
        {
            ValueWriter::WriteValue(aOut->endzoom, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGBallInstance>
{
    static void Read(const QDomAttr& aAttribute, WOGBallInstance* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("type"))
        {
            ValueWriter::WriteValue(aOut->type, aAttribute);
        }
        else if (name == QLatin1String("x"))
        {
            ValueWriter::WriteValue(aOut->x, aAttribute);
        }
        else if (name == QLatin1String("y"))
        {
            ValueWriter::WriteValue(aOut->y, aAttribute);
        }
        else if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("angle"))
        {
            ValueWriter::WriteValue(aOut->angle, aAttribute);
        }
        else if (name == QLatin1String("discovered"))
        {
            ValueWriter::WriteValue(aOut->discovered, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGPipe::WOGVertex>
{
    static void Read(const QDomAttr& aAttribute, WOGPipe::WOGVertex* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("x"))
        {
            ValueWriter::WriteValue(aOut->x, aAttribute);
        }
        else if (name == QLatin1String("y"))
        {
            ValueWriter::WriteValue(aOut->y, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGPipe>
{
    static void Read(const QDomAttr& aAttribute, WOGPipe* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("depth"))
        {
            ValueWriter::WriteValue(aOut->depth, aAttribute);
        }
        else if (name == QLatin1String("type"))
        {
            ValueWriter::WriteValue(aOut->type, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGStrand>
{
    static void Read(const QDomAttr& aAttribute, WOGStrand* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("gb1"))
        {
            ValueWriter::WriteValue(aOut->gb1, aAttribute);
        }
        else if (name == QLatin1String("gb2"))
        {
            ValueWriter::WriteValue(aOut->gb2, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGLevelExit>
{
    static void Read(const QDomAttr& aAttribute, WOGLevelExit* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("pos"))
        {
            ValueWriter::WriteValue(aOut->pos, aAttribute);
        }
        else if (name == QLatin1String("radius"))
        {
            ValueWriter::WriteValue(aOut->radius, aAttribute);
        }
        else if (name == QLatin1String("filter"))
        {
            ValueWriter::WriteValue(aOut->filter, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGLevel::Id>
{
    static void Read(const QDomAttr& aAttribute, WOGLevel::Id* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(*aOut, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGTargetHeight>
{
    static void Read(const QDomAttr& aAttribute, WOGTargetHeight* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("y"))
        {
            ValueWriter::WriteValue(aOut->y, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGSignpost>
{
    static void Read(const QDomAttr& aAttribute, WOGSignpost* aOut)
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
        else if (name == QLatin1String("text"))
        {
            ValueWriter::WriteValue(aOut->text, aAttribute);
        }
        else
        {
            AttributeReader<WOGVObject>::Read(aAttribute, aOut);
        }
    }
};

template<>
struct AttributeReader<WOGLevel>
{
    static void Read(const QDomAttr& aAttribute, WOGLevel* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("ballsrequired"))
        {
            ValueWriter::WriteValue(aOut->ballsrequired, aAttribute);
        }
        else if (name == QLatin1String("letterboxed"))
        {
            ValueWriter::WriteValue(aOut->letterboxed, aAttribute);
        }
        else if (name == QLatin1String("visualdebug"))
        {
            ValueWriter::WriteValue(aOut->visualdebug, aAttribute);
        }
        else if (name == QLatin1String("autobounds"))
        {
            ValueWriter::WriteValue(aOut->autobounds, aAttribute);
        }
        else if (name == QLatin1String("textcolor"))
        {
            ValueWriter::WriteValue(aOut->textcolor, aAttribute);
        }
        else if (name == QLatin1String("texteffects"))
        {
            ValueWriter::WriteValue(aOut->texteffects, aAttribute);
        }
        else if (name == QLatin1String("timebugprobability"))
        {
            ValueWriter::WriteValue(aOut->timebugprobability, aAttribute);
        }
        else if (name == QLatin1String("strandgeom"))
        {
            ValueWriter::WriteValue(aOut->strandgeom, aAttribute);
        }
        else if (name == QLatin1String("allowskip"))
        {
            ValueWriter::WriteValue(aOut->allowskip, aAttribute);
        }
    }
};
