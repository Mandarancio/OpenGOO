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

QPointF OGXmlConfig::StringToPoint(const QString & position)
{
    QStringList pos(position.split(","));

    if (pos.size() == 2)
    {
        return QPointF(pos.at(0).toDouble(), pos.at(1).toDouble());
    }
    else { return QPointF(); }
}

QPointF OGXmlConfig::StringToPoint(const QString & x, const QString & y)
{
    return QPointF(x.toDouble(), y.toDouble());
}

QColor OGXmlConfig::StringToColor(const QString & color)
{
    QStringList list = color.split(",");
    
    if (list.size() == 3)
    {
        return QColor(list.at(0).toInt()
                      , list.at(1).toInt()
                      , list.at(2).toInt());
    }
    else { return QColor(); }
}

bool OGXmlConfig::StringToBool(const QString & value)
{
    if (value == "true") { return true; }
    else { return false; }
}

QSizeF OGXmlConfig::StringToSize(const QString width, const QString height)
{
    return QSizeF(width.toDouble(), height.toDouble());
}
