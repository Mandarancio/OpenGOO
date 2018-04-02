#pragma once

#include <OGConfiguration>

class OGData
{
    public:
        static WOGLevel* GetLevel(const QString &path);
        static WOGScene* GetScene(const QString &path);
        static WOGResources* GetResources(const QString &path);
};
