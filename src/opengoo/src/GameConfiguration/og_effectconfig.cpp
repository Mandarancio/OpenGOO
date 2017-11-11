#include "og_effectconfig.h"
#include "ogeffectconfigparser.h"

struct OGEffectConfigHelper
{
    typedef OGEffectConfigParser Parser;

    static void AddElement(std::vector<WOGEffect::WOGParticle>& aData, const QDomElement& aElement)
    {
        aData.emplace_back(CreateParticle(aElement));
    }

    static void AddElement(WOGEffects& aData, const QDomElement& aElement)
    {
        aData.insert(aElement.attribute("name"), CreateEffect(aElement));
    }

    static void AddElement(std::vector<AxialSinOffset>& aData, const QDomElement& aElement)
    {
        aData.emplace_back(CreateAxialSinOffset(aElement));
    }

    static AxialSinOffset CreateAxialSinOffset(const QDomElement& aElement)
    {
        AxialSinOffset obj;

        auto attributes = aElement.attributes();
        for (int i = 0; i < attributes.count(); ++i)
        {
            Parser::WriteAttribute(obj, attributes.item(i).toAttr());
        }

        return obj;
    }

    static WOGEffect::WOGParticle CreateParticle(const QDomElement& aElement)
    {
        WOGEffect::WOGParticle obj;

        auto attributes = aElement.attributes();
        for (int i = 0; i < attributes.count(); ++i)
        {
            Parser::WriteAttribute(obj, attributes.item(i).toAttr());
        }

        for (auto node = aElement.firstChild(); !node.isNull(); node = node.nextSibling())
        {
            auto el = node.toElement();
            if (!el.isNull())
            {
                AddElement(obj.axialsinoffset, el);
            }
        }

        return obj;
    }

    static std::shared_ptr<WOGEffect> CreateEffect(const QDomElement& aElement)
    {
        auto obj = std::make_shared<WOGEffect>(Enumerator<WOGEffect::Type>::ToEnum(aElement.tagName()));

        auto attributes = aElement.attributes();
        for (int i = 0; i < attributes.count(); ++i)
        {
            Parser::WriteAttribute(*obj, attributes.item(i).toAttr());
        }

        for (auto node = aElement.firstChild(); !node.isNull(); node = node.nextSibling())
        {
            auto el = node.toElement();
            if (!el.isNull())
            {
                AddElement(obj->particle, el);
            }
        }

        return obj;
    }
};

OGEffectConfig::Type OGEffectConfig::Parser()
{
    auto obj = Type(new WOGEffects);
    for (auto node = rootElement.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        auto el = node.toElement();
        if (!el.isNull())
        {
            OGEffectConfigHelper::AddElement(*obj, el);
        }
    }

    return obj;
}
