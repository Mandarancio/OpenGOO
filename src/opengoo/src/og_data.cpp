#include "og_data.h"
#include "logger.h"
#include "wog_scene.h"
#include "wog_text.h"

template<class T> inline static typename T::Type GetData(const QString &path)
{
    T config(path);

    if (config.Open())
    {
        if (config.Read())
        {
            return config.Parser();
        }
        else
        {
            logWarn("File " + path + " is corrupted");
        }
    }
    else
    {
        logWarn("File " + path + " not found");
    }

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
    config.SetLanguage(lang);

    if (config.Open())
    {
        if (config.Read())
        {
            return config.Parser().release();
        }
        else
        {
            logWarn("File " + path + " is corrupted");
        }
    }
    else
    {
        logWarn("File " + path + " not found");
    }

    return nullptr;
}

WOGEffects* OGData::GetEffects(const QString &path)
{
    return GetData<OGEffectConfig>(path).release();
}
