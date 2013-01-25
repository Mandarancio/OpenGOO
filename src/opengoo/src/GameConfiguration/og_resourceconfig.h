#ifndef OG_RESOURCECONFIG_H
#define OG_RESOURCECONFIG_H

#include <og_xmlconfig.h>
#include "wog_resources.h"

class OGResourceConfig : public OGXmlConfig
{
    QString defaultPath_;
    QString defaultIdPrefix_;

public:
    OGResourceConfig(const QString & filename);

    WOGResources* Parser(QString groupid=QString());
    WOGResource* CreateResource(const QDomElement & element
                                , WOGResource::Type type
                                );
    WOGResourceGroup* CreateResourceGroup(const QDomElement & element);
};

#endif // OG_RESOURCECONFIG_H
