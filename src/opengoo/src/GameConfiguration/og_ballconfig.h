#pragma once

#include "balltagparser.h"

template<typename TTagParser>
class OGBallConfig : public OGXmlConfig
{
public:
    typedef typename TTagParser::Type Type;

public:
    OGBallConfig()
    {
        SetRootTag(TTagParser::rootTag);
    }

    Type* Parser()
    {
        auto obj = Type::Create();
        mTagParser.Parse(rootElement.tagName(), rootElement, obj.get());

        for (auto node = rootElement.firstChild(); !node.isNull(); node = node.nextSibling())
        {
            if (!node.isElement())
            {
                continue;
            }

            auto element = node.toElement();
            const auto tag = element.tagName();
            mTagParser.Parse(tag, element, obj.get());
        }

        return obj.release();
    }

//private:
//    void ReadAttributes(WOGBall* ball);
//    void CreateBehaviourAttributes_(WOGBall* ball);
//    void CreateMovementAttributes_(WOGBall* ball);
//    void CreatePlayerInteraction_(WOGBall* ball);
//    void CreateCosmeticAttributes_(WOGBall* ball);
//    void _CreateLevelInteraction(WOGBall* ball);

private:
    TTagParser mTagParser;
};
