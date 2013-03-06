#include "og_data.h"
#include "logger.h"

template<class T, class C> T* OGData::_GetData(const QString &path)
{
    C config(path);

    if (config.Open())
    {
        if (config.Read()) { return config.Parser(); }
        else { logWarn("File " + path + " is corrupted"); }
    }
    else { logWarn("File " + path + " not found"); }

    return 0;
}

WOGScene* OGData::GetScene(const QString &path)
{
    return _GetData<WOGScene, OGSceneConfig> (path);
}

WOGResources* OGData::GetResources(const QString &path)
{
    return _GetData<WOGResources, OGResourceConfig> (path);
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
