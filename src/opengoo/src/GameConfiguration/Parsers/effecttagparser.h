#pragma once

#include "effectattributereader.h"

template<>
struct TagParser<WOGEffects>
{
    typedef WOGEffects Type;

    constexpr static const char* rootTag = "effects";

    std::shared_ptr<WOGEffect> CreateEffect(const QString& aType)
    {
        if (aType == QLatin1String("particleeffect"))
        {
            return std::make_shared<WOGEffect>(WOGEffect::Type::e_point);
        }
        else if (aType == QLatin1String("ambientparticleeffect"))
        {
            return std::make_shared<WOGEffect>(WOGEffect::Type::e_ambient);
        }

        return nullptr;
    }

    void Parse(const QString& aTag, const QDomElement& aElement, WOGEffect::WOGParticle* aOut)
    {
        if (aTag == QLatin1String("axialsinoffset"))
        {
            aOut->axialsinoffset.push_back(WOGEffect::WOGParticle::WOGAxialSinOffset());
            auto& offset = aOut->axialsinoffset.back();
            read_attributes(aElement, &offset, mAttributeReader);
        }
    }

    void Parse(const QString& aTag, const QDomElement& aElement, WOGEffect* aOut)
    {
        if (aTag == QLatin1String("particle"))
        {
            aOut->particle.push_back(WOGEffect::WOGParticle());
            auto& particle = aOut->particle.back();
            read_attributes(aElement, &particle, mAttributeReader);
            parse_tags(aElement, &particle,
                       [this](const QString& tag, const QDomElement& element, WOGEffect::WOGParticle* out)
            {
                Parse(tag, element, out);
            });

        }
    }

    void Parse(const QString& aTag, const QDomElement& aElement, Type* aOut)
    {
        if (auto effect = CreateEffect(aTag))
        {
            read_attributes(aElement, effect.get(), mAttributeReader);
            parse_tags(aElement, effect.get(), [this](const QString& tag, const QDomElement& element, WOGEffect* out)
            {
                Parse(tag, element, out);
            });

            auto name = aElement.attribute("name");
            aOut->insert(name, effect);
        }
    }

private:
    AttributeReader<Type> mAttributeReader;
};
