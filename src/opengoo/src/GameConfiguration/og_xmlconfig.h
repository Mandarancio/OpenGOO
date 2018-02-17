#pragma once

#include <QFile>
#include <QDomDocument>
#include <QResource>
#include <QPoint>
#include <QColor>
#include <QSizeF>

#include "OGLib/optional.h"


class OGXmlConfig
{
public:
    void Close();
    bool Read();

    bool IsOpen() const
    {
        return file_.isOpen();
    }

    bool Open(const QString& aFileName);

    bool Load(const QString& aFileName)
    {
        Close();
        return (Open(aFileName) && Read());
    }

    const QString GetFilename() const
    {
        return file_.fileName();
    }

    void SetRootTag(const QString& root)
    {
        rootTag_ = root;
    }

    static QPointF StringToPointF(const QString& aPosition);
    static QPointF StringToPointF(const QString& x, const QString& y);
    static QPoint StringToPoint(const QString& aPosition);
    static QColor StringToColor(const QString& color);
    static bool StringToBool(const QString& value)
    {
        return (value == "true") ? true : false;
    }

    static QSizeF StringToSize(const QString width, const QString height);

    static void WriteValue(int& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value().toInt();
    }

    static void WriteValue(float& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value().toFloat();
    }

    static void WriteValue(QPointF& aData, const QDomAttr& aAttribute)
    {
        aData = StringToPointF(aAttribute.value());
    }

    static void WriteValue(QPoint& aData, const QDomAttr& aAttribute)
    {
        aData = StringToPoint(aAttribute.value());
    }

    static void WriteValue(QStringList& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value().split(",");
    }

    static void WriteValue(QString& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value();
    }

    static void WriteValue(oglib::Optional<bool>& aData, const QDomAttr& aAttribute)
    {
        aData = StringToBool(aAttribute.value());
    }

    static void WriteValue(oglib::Optional<QPointF>& aData, const QDomAttr& aAttribute)
    {
        aData = StringToPointF(aAttribute.value());
    }

    static void WriteValue(oglib::Optional<QColor>& aData, const QDomAttr& aAttribute)
    {
        aData = StringToColor(aAttribute.value());
    }

    static void WriteValue(oglib::Optional<float>& aData, const QDomAttr& aAttribute)
    {
        aData = aAttribute.value().toFloat();
    }

    static void WriteValue(bool& aData, const QDomAttr& aAttribute)
    {
        aData = StringToBool(aAttribute.value());
    }

    static void WriteValue(std::pair<bool, float>& aData, const QDomAttr& aAttribute)
    {
        aData.first = true;
        WriteValue(aData.second, aAttribute);
    }

    static void WriteValue(std::pair<bool, QPointF>& aData, const QDomAttr& aAttribute)
    {
        aData.first = true;
        WriteValue(aData.second, aAttribute);
    }

protected:
    OGXmlConfig()
    {
    }

    ~OGXmlConfig()
    {
        Close();
    }

    bool OpenFile(const QString& filename);

protected:
    QDomElement rootElement;

private:
    QFile file_;
    QDomDocument domDoc_;
    QString rootTag_;   
};
