#ifndef OG_MATERIALCONFIG_H
#define OG_MATERIALCONFIG_H

#include "og_xmlconfig.h"
#include "wog_material.h"

#include <QList>

class OGMaterialConfig : public OGXmlConfig
{
public:
    OGMaterialConfig(const QString & filename);
    WOGMaterialList* Parser();

    WOGMaterial* CreateMaterial(const QDomElement & element);
};

#endif // OG_MATERIALCONFIG_H
