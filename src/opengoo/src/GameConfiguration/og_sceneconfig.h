#ifndef OG_SCENECONFIG_H
#define OG_SCENECONFIG_H

#include "og_xmlconfig.h"

#include <QColor>

struct OGSceneLayer
{
    OGSceneLayer(QString image, float scalex, float scaley, float x, float y)
        : image(image)
        , scalex(scalex)
        , scaley(scaley)
        , x(x)
        , y(y) {}

    QString image;
    qreal scalex;
    qreal scaley;
    qreal x;
    qreal y;
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
