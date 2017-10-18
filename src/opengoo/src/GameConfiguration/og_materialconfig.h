#pragma once

#include "og_xmlconfig.h"
#include "wog_material.h"

class OGMaterialConfig : public OGXmlConfig
{
public:
    typedef WOGMaterialList Type;

public:
    OGMaterialConfig(const QString & filename)
        : OGXmlConfig(filename)
    {
        SetRootTag("materials");
    }

    WOGMaterialList Parser();
};
