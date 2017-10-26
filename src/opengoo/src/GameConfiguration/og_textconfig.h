#pragma once

#include "og_xmlconfig.h"

struct WOGText;

class OGTextConfig : public OGXmlConfig
{
public:
    typedef std::unique_ptr<WOGText> Type;

public:
    OGTextConfig(const QString& aFilename)
        :OGXmlConfig(aFilename)
    {
        SetRootTag("strings");
    }

    void SetLanguage(const QString& aLanguage)
    {
        mLanguage = aLanguage;
    }

    Type Parser();

private:
    QString mLanguage;
};
