#pragma once

#include "og_xmlconfig.h"

struct WOGLevel;

class OGLevelConfig : public OGXmlConfig
{
public:
    typedef std::unique_ptr<WOGLevel> Type;

public:
    OGLevelConfig()
    {
        SetRootTag("level");
    }

    Type Parser();
};
