#ifndef OG_TEXTCONFIG_H
#define OG_TEXTCONFIG_H

#include "og_xmlconfig.h"

struct OGString
{
    QString id;
    QString text;
    QString es;
    QString fr;
    QString de;
    QString it;
};

typedef  QList<OGString> OGStringList;

class OGTextConfig : public OGXmlConfig
{
public:
    OGTextConfig(const QString & filename);

    OGStringList Parser();
};

#endif // OG_TEXTCONFIG_H
