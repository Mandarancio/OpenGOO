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

    static QPointF StringToPoint(const QString& position);
    static QPointF StringToPoint(const QString& x, const QString& y);
    static QColor StringToColor(const QString& color);
    static bool StringToBool(const QString& value)
    {
        return (value == "true") ? true : false;
    }

    static QSizeF StringToSize(const QString width, const QString height);

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
