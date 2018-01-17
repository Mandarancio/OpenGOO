#pragma once

#include "og_xmlconfig.h"

struct OGConfig
{
    int screen_width;
    int screen_height;
    int refreshrate;
    bool fullscreen;
    bool isCrt;
    QString language;
    QString levelName;

    OGConfig()
        : screen_width(0)
        , screen_height(0)
        , refreshrate(0)
        , fullscreen(false)
        , isCrt(false)
    {
    }
};

class OGGameConfig : public OGXmlConfig
{
public:
    OGGameConfig()
    {
        SetRootTag("config");
    }

    OGConfig Parser();

    static void Create(const QString& fileName, OGConfig& config); // Create new game configuration file
};
