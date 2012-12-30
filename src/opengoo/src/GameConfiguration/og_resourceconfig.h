#ifndef OG_RESOURCECONFIG_H
#define OG_RESOURCECONFIG_H

#include <og_xmlconfig.h>
#include "og_resources.h"

class OGResourceConfig : public OGXmlConfig
{
public:
    OGResourceConfig(const QString & filename);

    OGResources Parser(QString groupid=QString());
};

#endif // OG_RESOURCECONFIG_H
