#pragma once

#include "wog_resources.h"

struct ResourceDefaults
{
    QString path;
    QString idprefix;

    ResourceDefaults()
        : path("./")
    {
    }

    void Init()
    {
        path = "./";
        idprefix = "";
    }
};

struct ResourceAttributes
{
    QString id;
    QString path;
    QString jp;
};

template<>
struct AttributeReader<ResourceDefaults>
{
    static void Read(const QDomAttr& aAttribute, ResourceDefaults* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("idprefix"))
        {
            ValueWriter::WriteValue(aOut->idprefix, aAttribute);
        }
        else if (name == QLatin1String("path"))
        {
            ValueWriter::WriteValue(aOut->path, aAttribute);
        }
    }
};

template<>
struct AttributeReader<ResourceAttributes>
{
    static void Read(const QDomAttr& aAttribute, ResourceAttributes* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("path"))
        {
            ValueWriter::WriteValue(aOut->path, aAttribute);
        }
        else if (name == QLatin1String("jp"))
        {
            ValueWriter::WriteValue(aOut->jp, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGResourceGroup>
{
    static void Read(const QDomAttr& aAttribute, WOGResourceGroup* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
    }
};
