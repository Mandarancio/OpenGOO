#pragma once

#include "og_xmlconfig.h"

struct WOGScene;

class OGSceneConfig : public OGXmlConfig<DefaultTagParser>
{
public:
    typedef std::unique_ptr<WOGScene> Type;

public:
    OGSceneConfig()
    {
        SetRootTag("scene");
    }

    Type Parser();
};
