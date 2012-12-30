#ifndef OG_XMLCONFIG_H
#define OG_XMLCONFIG_H

#include <QFile>
#include <QDomDocument>
#include <QResource>
#include <QPoint>
#include <QColor>

class OGXmlConfig
{
public:
    OGXmlConfig(const QString & filename=QString());
    virtual ~OGXmlConfig();

    bool Open();
    void Close();
    bool Read();

    inline QString GetFilename() { return fileName_; }
    inline void SetFileName(const QString & filename) { fileName_ = filename; }
    inline void SetRootTag(const QString & root) { rootTag_ = root; }
    QPointF StringToPoint(const QString & position);
    QPointF StringToPoint(const QString & x, const QString & y);
    QColor StringToColor(const QString & color);
    bool StringToBool (const QString & value);

protected:
    QDomElement rootElement;

private:
    bool isOpen_;
    QString fileName_;
    QFile file_;
    QDomDocument domDoc_;
    QString rootTag_;
};

#endif // OG_XMLCONFIG_H
