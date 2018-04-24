#include "og_data.h"
#include "logger.h"
#include "wog_scene.h"

template<class T> inline static typename T::Type GetData(const QString &path)
{
    T config;

    if (config.Open(path))
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

template<class T>
static typename T::Type ReadConfig(T& config, const QString &path)
{
    if (!config.Load(path))
    {
        logWarn(QLatin1String("Could not load file ") + path);
        return nullptr;
    }

    return config.Parser();
}

WOGResources* OGData::GetResources(const QString &path)
{
    return GetData<OGResourceConfig>(path);
}
