#include "og_data.h"
#include "logger.h"
#include "wog_scene.h"

template<class T> static typename T::Type GetData(const QString &path)
{
    T config(path);

    if (config.Open())
    {
        if (config.Read()) { return config.Parser(); }
        else { logWarn("File " + path + " is corrupted"); }
    }
    else { logWarn("File " + path + " not found"); }

    return nullptr;
}

WOGScene* OGData::GetScene(const QString &path)
{
    return GetData<OGSceneConfig>(path).release();
}

WOGResources* OGData::GetResources(const QString &path)
{
    return GetData<OGResourceConfig>(path);
}

WOGText* OGData::GetText(const QString &path, const QString &lang)
{
    OGTextConfig config(path);

    if (config.Open())
    {
        if (config.Read()) { return config.Parser(lang); }
        else { logWarn("File " + path + " is corrupted"); }
    }
    else { logWarn("File " + path + " not found"); }

    return 0;
}
