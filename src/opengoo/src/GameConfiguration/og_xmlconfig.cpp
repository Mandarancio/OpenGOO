#include "og_xmlconfig.h"

OGXmlConfig::OGXmlConfig(const QString & filename)
{
    fileName_ = filename;
    isOpen_ = false;
}

OGXmlConfig::~OGXmlConfig()
{
    Close();
}

bool OGXmlConfig::Open()
{
    if (isOpen_) { return false; }

    file_.setFileName(fileName_);

    if (file_.open(QIODevice::ReadOnly))
    {
        isOpen_ = true;
        return true;
    }
    else { return false; }
}

void OGXmlConfig::Close()
{
    if (isOpen_)
    {
        file_.close();
        isOpen_ = false;
    }
}

bool OGXmlConfig::Read()
{
    if (domDoc_.setContent(&file_))
    {
        rootElement = domDoc_.documentElement();

        if (rootElement.tagName() == rootTag_) { return true; }
    }

    return false;
}

