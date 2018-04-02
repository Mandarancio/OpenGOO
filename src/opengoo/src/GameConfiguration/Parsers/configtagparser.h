#pragma once

#include "configattributereader.h"

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
        QDomElement el;
        el.setAttribute(aName, aValue);
        return el.attributeNode(aName);
    }

private:
    AttributeReader<Type> mAttributeReader;
};
