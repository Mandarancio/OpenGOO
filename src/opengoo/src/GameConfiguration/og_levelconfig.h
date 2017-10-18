#pragma once

#include "og_xmlconfig.h"

struct WOGLevel;

class OGLevelConfig : public OGXmlConfig
{
public:
    typedef WOGLevel Type;

public:
    OGLevelConfig(const QString& filename)
        : OGXmlConfig(filename)
    {
        SetRootTag("level");
    }

    WOGLevel Parser();
};
