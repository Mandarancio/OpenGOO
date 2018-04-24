#pragma once

#include "ballattributereader.h"

template<>
struct TagParser<WOGBall>
{
    typedef WOGBall Type;

    constexpr static const char* rootTag = "ball";

    void Parse(const QString& aTag, const QDomElement& aElement, WOGBall* aOut)
    {
        if (aTag == QLatin1String("ball"))
        {
            read_attributes(aElement, &aOut->attribute, mAttributeReader);
        }
        else if (aTag == QLatin1String("strand"))
        {
            aOut->strand.reset(new WOGBallStrand);
            Parse(aElement, aOut->strand.get());
        }
        else if (aTag == QLatin1String("detachstrand"))
        {
            aOut->detachstrand.reset(new WOGBallDetachstrand);
            Parse(aElement, aOut->detachstrand.get());
        }
        else if (aTag == QLatin1String("part"))
        {
            aOut->parts.push_back(WOGPart());
            read_attributes(aElement, &aOut->parts.last(), mAttributeReader);
        }
        else if (aTag == QLatin1String("marker"))
        {
            read_attributes(aElement,&aOut->marker, mAttributeReader);
        }
        else if (aTag == QLatin1String("shadow"))
        {
        }
        else if (aTag == QLatin1String("particles"))
        {
        }
        else if (aTag == QLatin1String("splat"))
        {
        }
        else if (aTag == QLatin1String("sound"))
        {
            Parse(aElement, &aOut->sounds);
        }
        else if (aTag == QLatin1String("sinvariance"))
        {
        }
    }

private:
    static void Parse(const QDomElement& aElement, QMap<QString, QStringList>* aOut)
    {
        aOut->value(aElement.attribute(QLatin1String("event")), aElement.attribute(QLatin1String("id")).split(","));
    }

    static void Parse(const QDomElement& aElement, WOGBallStrand* aOut)
    {
        aOut->burntimage = aElement.attribute("burntimage");
        aOut->fireparticles = aElement.attribute("fireparticles");
        aOut->image = aElement.attribute("image");
        aOut->inactiveimage = aElement.attribute("inactiveimage");
        aOut->type = aElement.attribute("type");

        aOut->burnspeed = aElement.attribute("burnspeed", "0").toFloat();
        aOut->dampfac = aElement.attribute("dampfac", "0").toFloat();
        aOut->ignitedelay = aElement.attribute("ignitedelay", "0").toFloat();
        aOut->maxforce = aElement.attribute("maxforce", "0").toFloat();
        aOut->maxlen1 = aElement.attribute("maxlen1", "0").toFloat();
        aOut->maxlen2 = aElement.attribute("maxlen2", "0").toFloat();
        aOut->minlen = aElement.attribute("minlen", "0").toFloat();
        aOut->shrinklen = aElement.attribute("shrinklen", "140").toFloat();
        aOut->springconstmax = aElement.attribute("springconstmax").toFloat();
        aOut->springconstmin = aElement.attribute("springconstmin").toFloat();
        aOut->thickness = aElement.attribute("thickness").toFloat();

        aOut->rope = ValueWriter::StringToBool(aElement.attribute("rope"));
        aOut->geom = ValueWriter::StringToBool(aElement.attribute("geom"));
        aOut->walkable = ValueWriter::StringToBool(aElement.attribute("walkable", "true"));
    }

    static void Parse(const QDomElement& aElement, WOGBallDetachstrand* aOut)
    {
        aOut->image = aElement.attribute("image");
        aOut->maxlen = aElement.attribute("maxlen", "0").toFloat();
    }

private:
    AttributeReader<Type> mAttributeReader;
};
