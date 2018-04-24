#pragma once

#include "levelattributereader.h"

template<>
struct TagParser<WOGLevel>
{
    typedef WOGLevel Type;

    constexpr static const char* rootTag = "level";

    void Parse(const QString& aTag, const QDomElement& aElement, WOGLevel* aOut)
    {
        if (aTag == QLatin1String("level"))
        {
            read_attributes(aElement, aOut);
        }
        else if (aTag == QLatin1String("camera"))
        {
            AddCamera(aElement, aOut);
        }
        else if (aTag == QLatin1String("ballinstance"))
        {
            AddElement(aElement, aOut->ball);
        }
        else if (aTag == QLatin1String("strand"))
        {
            AddElement(aElement, aOut->strand);
        }
        else if (aTag == QLatin1String("pipe"))
        {
            AddPipe(aElement, aOut);
        }
        else if (aTag == QLatin1String("levelexit"))
        {
            AddElement(aElement, aOut->levelexit);
        }
        else if (aTag == QLatin1String("loopsound"))
        {
            AddElement(aElement, aOut->loopsound);
        }
        else if (aTag == QLatin1String("music"))
        {
            read_attributes(aElement, &aOut->music);
        }
        else if (aTag == QLatin1String("signpost"))
        {
            AddElement(aElement, aOut->signpost);
        }
        else
        {
            qDebug() << aTag;
        }
    }

private:
    static void AddCamera(const QDomElement& aElement, WOGLevel* aOut)
    {
        auto& el = AddElement(aElement, aOut->camera);
        parse_tags(aElement, &el, [](const QString& aTag, const QDomElement& aElement, WOGCamera* aOut)
        {
            if (aTag == QLatin1String("poi"))
            {
                AddElement(aElement, aOut->poi);
            }
        });
    }

    static void AddPipe(const QDomElement& aElement, WOGLevel* aOut)
    {
        auto& el = AddElement(aElement, aOut->pipe);
        parse_tags(aElement, &el, [](const QString& aTag, const QDomElement& aElement, WOGPipe* aOut)
        {
            if (aTag == QLatin1String("vertex"))
            {
                AddElement(aElement, aOut->vertex);
            }
        });
    }
};
