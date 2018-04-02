#pragma once

#include <QString>

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
        : screen_width(800)
        , screen_height(600)
        , refreshrate(60)
        , fullscreen(false)
        , isCrt(false)
        , language("en")
    {
    }

    static std::unique_ptr<OGConfig> Create()
    {
        return std::unique_ptr<OGConfig>(new OGConfig);
    }

    void Write(const QString& aFileName); // Create new game configuration file

    bool Load(const QString& aFileName);
};
