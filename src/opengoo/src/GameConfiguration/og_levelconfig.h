#ifndef OG_LEVELCONFIG_H
#define OG_LEVELCONFIG_H

#include "og_xmlconfig.h"
#include <QPoint>

struct OGCamera
{
    QString aspect;
    QPointF pos;
};

struct OGLevel
{
    QList <OGCamera> cameras;
    QString music;
};

class OGLevelConfig : public OGXmlConfig
{
public:
    OGLevelConfig(const QString &filename);

    void Parser(OGLevel & level);
};

#endif // OG_LEVELCONFIG_H
