#pragma once

#include "wog_material.h"
#include "og_materialconfig.h"

template<>
struct TagParser<WOGMaterialList>
{
    typedef WOGMaterialList Type;

    constexpr static const char* rootTag = "materials";

    void Parse(const QString& aTag, const QDomElement& aElement, WOGMaterialList* aOut)
    {
        if (aTag == QLatin1String(rootTag))
        {
            OGMaterialConfig::Parser(aElement, aOut);
        }
    }
};


