#ifndef WOG_TEXTCONFIG_H
#define WOG_TEXTCONFIG_H

#include "og_xmlconfig.h"
#include "wog_text.h"

class OGTextConfig : public OGXmlConfig
{
public:
    OGTextConfig(const QString & filename);

    WOGText* Parser(const QString & language);
    WOGString* CreateString(const QDomElement & element
                            , const QString & language
                            );
};

#endif // WOG_TEXTCONFIG_H
