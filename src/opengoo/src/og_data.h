#pragma once

#include <OGConfiguration>

class OGData
{
    public:
        static WOGLevel* GetLevel(const QString &path);
        static WOGScene* GetScene(const QString &path);
        static WOGText *GetText(const QString &path, const QString &lang);
        static WOGResources* GetResources(const QString &path);
        static WOGEffects* GetEffects(const QString &path);
};
