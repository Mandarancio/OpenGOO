#pragma once

#include "configattributereader.h"

#include <QDebug>
#include <QDomAttr>

template<>
struct TagParser<OGConfig>
{
    typedef OGConfig Type;

    constexpr static const char* rootTag = "config";

    void Parse(const QString& aTag, const QDomElement& aElement, Type* aOut)
    {
        if (aTag == QLatin1String("param"))
        {
            auto name = aElement.attribute("name");
            auto value = aElement.attribute("value");
            auto attr = BuildAttribute(name, value);
            mAttributeReader.Read(attr, aOut);
        }
    }

private:
    QDomAttr BuildAttribute(const QString& aName, const QString& aValue)
    {
        auto attr = mDoc.createAttribute(aName);
        attr.setValue(aValue);
        return attr;
    }

private:
    AttributeReader<Type> mAttributeReader;
    QDomDocument mDoc;
};
