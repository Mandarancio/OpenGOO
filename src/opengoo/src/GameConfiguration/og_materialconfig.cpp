#include "og_materialconfig.h"

#include <QDebug>

OGMaterialConfig::OGMaterialConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("materials");
}

WOGMaterialList* OGMaterialConfig::Parser()
{
    qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

    return 0;
}
