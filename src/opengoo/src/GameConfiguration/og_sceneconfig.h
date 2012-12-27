#ifndef OG_SCENECONFIG_H
#define OG_SCENECONFIG_H

#include "og_xmlconfig.h"

#include <QColor>

struct OGSceneLayer
{
    QString id;
    QString name;
    qreal depth;
    qreal x;
    qreal y;
    qreal scalex;
    qreal scaley;
    qreal rotation;
    qreal alpha;
    QColor colorize;
    QString image;
    QString anim;
    qreal animspeed;
};

struct OGScene
{
    qreal minx;
    qreal miny;
    qreal maxx;
    qreal maxy;
    QColor backgroundcolor;
    QList <OGSceneLayer> sceneLayers;
};

class OGSceneConfig : public OGXmlConfig
{
public:
    OGSceneConfig(const QString & filename);

    void Parser(OGScene & scene);
};

#endif // OG_SCENECONFIG_H
