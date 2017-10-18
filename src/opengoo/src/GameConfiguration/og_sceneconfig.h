#pragma once

#include "og_xmlconfig.h"

struct WOGScene;

class OGSceneConfig : public OGXmlConfig
{
public:
    typedef WOGScene* Type;

public:
    OGSceneConfig(const QString& filename)
        : OGXmlConfig(filename)
    {
        SetRootTag("scene");
    }

    WOGScene* Parser();
};
