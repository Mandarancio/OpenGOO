#include "og_sceneconfig.h"

OGSceneConfig::OGSceneConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("scene");
}

void OGSceneConfig::Parser(OGScene & scene)
{
    scene.minx = rootElement.attribute("minx", "").toDouble();
    scene.miny = rootElement.attribute("miny", "").toDouble();
    scene.maxx = rootElement.attribute("maxx", "").toDouble();
    scene.maxy = rootElement.attribute("maxy", "").toDouble();
    QString backgroundcolor = rootElement.attribute("backgroundcolor", "");
    QStringList bgList = backgroundcolor.split(",");
    QColor bg(bgList.at(0).toInt(), bgList.at(1).toInt(), bgList.at(2).toInt());
    scene.backgroundcolor = bg;

    for(QDomNode n = rootElement.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement domElement = n.toElement();

        if (domElement.tagName() == "SceneLayer")
        {
            QString image = domElement.attribute("image", "");
            float scalex = domElement.attribute("scalex", "").toDouble();
            float scaley = domElement.attribute("scaley", "").toDouble();
            float x = domElement.attribute("x", "").toDouble();
            float y = domElement.attribute("y", "").toDouble();

            scene.sceneLayers << OGSceneLayer(image, scalex, scaley, x, y);
        }
    }
}
