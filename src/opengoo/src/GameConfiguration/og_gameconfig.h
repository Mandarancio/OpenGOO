#pragma once

#include "og_xmlconfig.h"

struct OGConfig
{
    int screen_width;
    int screen_height;
    int refreshrate;
    bool fullscreen;
    QString language;

    OGConfig()
    {
        fullscreen = false;
        refreshrate = 0;
        screen_height = 0;
        screen_width = 0;
    }
};

class OGGameConfig : public OGXmlConfig
{
public:
    OGGameConfig(const QString & filename);
    OGConfig Parser();
    void Create(OGConfig &config); // Create new game configuration file
};
