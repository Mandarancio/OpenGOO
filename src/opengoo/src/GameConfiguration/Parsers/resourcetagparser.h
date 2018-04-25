#pragma once

#include "resourceattributereader.h"

template<>
struct TagParser<WOGResources>
{
    typedef WOGResources Type;

    constexpr static const char* rootTag = "ResourceManifest";

    void Parse(const QString& aTag, const QDomElement& aElement, WOGResources* aOut)
    {
        if (aTag == QLatin1String("resourcemanifest"))
        {
            mDefaults.Init();
        }
        else if (aTag == QLatin1String("resources"))
        {
            AddGroup(aElement, aOut);
        }
    }

private:
    void AddGroup(const QDomElement& aElement, WOGResources* aOut)
    {
        auto& el = AddElement(aElement, aOut->group);
        parse_tags(aElement, &el, [this](const QString& aTag, const QDomElement& aElement, WOGResourceGroup* aOut)
        {
            WOGResource* res = nullptr;
            if (aTag == QLatin1String("image"))
            {
                res = aOut->CreateImage();
            }
            else if (aTag == QLatin1String("sound"))
            {
                res = aOut->CreateSound();
            }
            else if (aTag == QLatin1String("font"))
            {
                res = aOut->CreateFont();
            }
            else if (aTag == QLatin1String("setdefaults"))
            {
                read_attributes(aElement, &mDefaults);
            }

            if (res)
            {
                ResourceAttributes attrs;
                read_attributes(aElement, &attrs);
                res->id = mDefaults.idprefix + attrs.id;
                res->path = mDefaults.path + attrs.path;
                res->jp = attrs.jp;
            }
        });
    }

private:
    ResourceDefaults mDefaults;
};
