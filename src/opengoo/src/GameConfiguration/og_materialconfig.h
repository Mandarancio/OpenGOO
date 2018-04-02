#pragma once

#include "og_xmlconfig.h"
#include "wog_material.h"

class OGMaterialConfig : public OGXmlConfig<DefaultTagParser>
{
public:
    typedef WOGMaterialList Type;

public:
    OGMaterialConfig()
    {
        SetRootTag("materials");
    }

    WOGMaterialList Parser();
};
