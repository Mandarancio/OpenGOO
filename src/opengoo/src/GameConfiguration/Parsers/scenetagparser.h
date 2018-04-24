#pragma once

#include "sceneattributereader.h"

template<>
struct TagParser<WOGScene>
{
    typedef WOGScene Type;

    constexpr static const char* rootTag = "scene";

    void Parse(const QString& aTag, const QDomElement& aElement, WOGScene* aOut)
    {
        if (aTag == QLatin1String("scene"))
        {
            read_attributes(aElement, aOut, mAttributeReader);
        }
        else if (aTag == QLatin1String("SceneLayer"))
        {
            AddElement(aElement, aOut->sceneLayer);
        }
        else if (aTag == QLatin1String("label"))
        {
            AddElement(aElement, aOut->label);
        }
        else if (aTag == QLatin1String("buttongroup"))
        {
            AddButtonGroup(aElement, aOut->buttongroup);
        }
        else if (aTag == QLatin1String("button"))
        {
            AddElement(aElement, aOut->button);
        }
        else if (aTag == QLatin1String("radialforcefield"))
        {
            AddElement(aElement, aOut->radialforcefield);
        }
        else if (aTag == QLatin1String("linearforcefield"))
        {
            AddElement(aElement, aOut->linearforcefield);
        }
        else if (aTag == QLatin1String("particles"))
        {
            AddElement(aElement, aOut->particle);
        }
        else if (aTag == QLatin1String("circle"))
        {
            AddElement(aElement, aOut->circle);
        }
        else if (aTag == QLatin1String("line"))
        {
            AddElement(aElement, aOut->line);
        }
        else if (aTag == QLatin1String("rectangle"))
        {
            AddElement(aElement, aOut->rectangle);
        }
        else if (aTag == QLatin1String("compositegeom"))
        {
            AddCompositeGeom(aElement, aOut->compositegeom);
        }
        else if (aTag == QLatin1String("hinge"))
        {
            AddElement(aElement, aOut->hinge);
        }
        else if (aTag == QLatin1String("motor"))
        {
            AddElement(aElement, aOut->motor);
        }
    }

private:
    static void AddCompositeGeom(const QDomElement& aElement, std::list<WOGCompositeGeom>& aList)
    {
        auto& el = AddElement(aElement, aList);;
        parse_tags(aElement, &el, [](const QString& aTag, const QDomElement& aElement, WOGCompositeGeom* aOut)
        {
            if (aTag == QLatin1String("circle"))
            {
                AddElement(aElement, aOut->circle);
            }
            else if (aTag == QLatin1String("rectangle"))
            {
                AddElement(aElement, aOut->rectangle);
            }
        });
    }

    static void AddButtonGroup(const QDomElement& aElement, std::list<WOGButtonGroup>& aList)
    {
        auto& el = AddElement(aElement, aList);;
        parse_tags(aElement, &el, [](const QString& aTag, const QDomElement& aElement, WOGButtonGroup* aOut)
        {
            if (aTag == QLatin1String("button"))
            {
                AddElement(aElement, aOut->button);
            }
        });
    }

private:
    AttributeReader<WOGScene> mAttributeReader;
};
