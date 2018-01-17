#include "og_xmlconfig.h"
#include "decrypter.h"
#include <QFileInfo>

bool OGXmlConfig::OpenFile(const QString& filename)
{
    file_.setFileName(filename);
    return file_.open(QIODevice::ReadOnly);
}

bool OGXmlConfig::Open(const QString& aFileName)
{
    if (QFile::exists(aFileName))
    {
        return OpenFile(aFileName);
    }

    if (QFile::exists(aFileName + ".xml"))
    {
        return OpenFile(aFileName + ".xml");
    }

    if (QFile::exists(aFileName + ".bin"))
    {
        return OpenFile(aFileName + ".bin");
    }

    return false;
}

void OGXmlConfig::Close()
{
    file_.close();
}

bool OGXmlConfig::Read()
{
    QFileInfo fi(file_.fileName());
    bool status = false;

    if (fi.suffix() == "bin")
    {
        QByteArray cryptedData = file_.readAll();
        auto xml_data = Decrypter().decrypt(cryptedData);
        QString str(xml_data.data());
        status = domDoc_.setContent(str);
    }
    else
    {
        status = domDoc_.setContent(&file_);
    }

    if (status)
    {
        rootElement = domDoc_.documentElement();

        if (rootElement.tagName() == rootTag_)
        {
            return true;
        }
    }

    return false;
}

QPointF OGXmlConfig::StringToPointF(const QString& aPosition)
{
    QPointF pos;
    auto sl = aPosition.split(",");
    for (int i = 0; i < sl.size(); ++i)
    {
        auto val = sl.at(i).toDouble();
        switch (i)
        {
        case 0:
            pos.setX(val);
            break;
        case 1:
            pos.setY(val);
            return pos;
        }
    }

    return pos;
}

QPointF OGXmlConfig::StringToPointF(const QString & x, const QString & y)
{
    return QPointF(x.toDouble(), y.toDouble());
}

QPoint OGXmlConfig::StringToPoint(const QString& aPosition)
{
    QPoint pos;
    auto sl = aPosition.split(",");
    for (int i = 0; i < sl.size(); ++i)
    {
        auto val = sl.at(i).toInt();
        switch (i)
        {
        case 0:
            pos.setX(val);
            break;
        case 1:
            pos.setY(val);
            return pos;
        }
    }

    return pos;
}

QColor OGXmlConfig::StringToColor(const QString & color)
{
    QStringList list = color.split(",");
    
    if (list.size() == 3)
    {
        return QColor(list.at(0).toInt(), list.at(1).toInt(), list.at(2).toInt());
    }
    else
    {
        return QColor();
    }
}

QSizeF OGXmlConfig::StringToSize(const QString width, const QString height)
{
    return QSizeF(width.toDouble(), height.toDouble());
}
