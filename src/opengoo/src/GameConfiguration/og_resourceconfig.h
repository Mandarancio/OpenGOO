#ifndef OG_RESOURCECONFIG_H
#define OG_RESOURCECONFIG_H

#include <og_xmlconfig.h>

struct OGResource
{    
    enum Type {IMAGE, SOUND, FONT};

    OGResource(Type type, QString id, QString path)
        : type(type), id(id), path(path) {}

    Type type;
    QString id;
    QString path;
};

class OGResourceConfig : public OGXmlConfig
{
public:
    OGResourceConfig(const QString & filename);

    void Parser(QList <OGResource > & resources);
};

#endif // OG_RESOURCECONFIG_H
