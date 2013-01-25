#ifndef OG_EFFECTCONFIG_H
#define OG_EFFECTCONFIG_H

#include "og_xmlconfig.h"
#include "wog_effects.h"

class OGEffectConfig : public OGXmlConfig
{
public:
    OGEffectConfig(const QString & filename);

    WOGEffects* Parser();
};

#endif // OG_EFFECTCONFIG_H
