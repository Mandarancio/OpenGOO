#pragma once

#include "og_xmlconfig.h"
#include "wog_effects.h"

class OGEffectConfig : public OGXmlConfig
{
public:
    typedef std::unique_ptr<WOGEffects> Type;

public:
    OGEffectConfig(const QString & aFileName)
        : OGXmlConfig(aFileName)
    {
        SetRootTag("effects");
    }

    Type Parser();
};
