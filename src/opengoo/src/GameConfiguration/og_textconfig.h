#pragma once
#include "og_xmlconfig.h"

struct WOGText;

class OGTextConfig : public OGXmlConfig
{
public:
    typedef WOGText* Type;

public:
    OGTextConfig(const QString& filename)
        :OGXmlConfig(filename)
    {
        SetRootTag("strings");
    }

    WOGText* Parser(const QString& language);
};
