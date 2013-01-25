#include "og_effectconfig.h"

#include <QDebug>

OGEffectConfig::OGEffectConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("effects");
}

WOGEffects* OGEffectConfig::Parser()
{
    qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

    return 0;
}
