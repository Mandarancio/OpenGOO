#ifndef OG_TEXTCONFIG_H
#define OG_TEXTCONFIG_H

#include "og_xmlconfig.h"

struct OGText
{
    OGText(QString id, QString text) : id(id), text(text) {}

    QString id;
    QString text;
};

class OGTextConfig : public OGXmlConfig
{
public:
    OGTextConfig(const QString & filename);

    void Parser(QList <OGText > & strings);
};

#endif // OG_TEXTCONFIG_H
