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
    QStringList list = rootElement.attribute("backgroundcolor", "").split(",");

    scene.backgroundcolor = QColor(
                list.at(0).toInt()
                , list.at(1).toInt()
                , list.at(2).toInt()
                );

    for(QDomNode n=rootElement.firstChild(); !n.isNull(); n=n.nextSibling())
    {
        QDomElement domElement = n.toElement();

        if (domElement.tagName() == "SceneLayer")
        {
            OGSceneLayer sceneLayer;

            sceneLayer.id = domElement.attribute("id", "");
            sceneLayer.name = domElement.attribute("name", "");
            sceneLayer.depth = domElement.attribute("depth", "").toDouble();
            sceneLayer.x = domElement.attribute("x", "").toDouble();
            sceneLayer.y = domElement.attribute("y", "").toDouble();
            sceneLayer.scalex = domElement.attribute("scalex", "").toDouble();
            sceneLayer.scaley = domElement.attribute("scaley", "").toDouble();

            sceneLayer.rotation = domElement.attribute(
                        "rotation"
                        , ""
                        ).toDouble();

            sceneLayer.alpha = domElement.attribute("alpha", "").toDouble();

            QStringList list = domElement.attribute("colorize", "").split(",");

            sceneLayer.colorize = QColor(
                        list.at(0).toInt()
                        , list.at(1).toInt()
                        , list.at(2).toInt()
                        );

            sceneLayer.image = domElement.attribute("image", "");           
            sceneLayer.anim = domElement.attribute("anim", "");

            sceneLayer.animspeed = domElement.attribute(
                        "animspeed"
                        , ""
                        ).toDouble();

            scene.sceneLayers << sceneLayer;
        }
    }
}
