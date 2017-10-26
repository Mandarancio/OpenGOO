#pragma once

#include <QFile>
#include <QDomDocument>
#include <QResource>
#include <QPoint>
#include <QColor>
#include <QSizeF>

class OGXmlConfig
{
public:
    OGXmlConfig(const QString& filename=QString())
        : isOpen_(false)
        , fileName_(filename)
    {
    }

    bool Open();
    void Close();
    bool Read();

    QString GetFilename()
    {
        return fileName_;
    }

    void SetFileName(const QString& filename)
    {
        fileName_ = filename;
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

    static void WriteValue(bool& aData, const QDomAttr& aAttribute)
    {
        aData = StringToBool(aAttribute.value());
    }

protected:
    ~OGXmlConfig()
    {
        Close();
    }

protected:
    QDomElement rootElement;

private:
    bool isOpen_;    
    QString fileName_;
    QFile file_;
    QDomDocument domDoc_;
    QString rootTag_;   
};
