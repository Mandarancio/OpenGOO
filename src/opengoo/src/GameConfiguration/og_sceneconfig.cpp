#include "og_sceneconfig.h"

OGSceneConfig::OGSceneConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("scene");
}

OGScene OGSceneConfig::Parser()
{
    OGScene scene;
    scene.minx = rootElement.attribute("minx").toDouble();
    scene.miny = rootElement.attribute("miny").toDouble();
    scene.maxx = rootElement.attribute("maxx").toDouble();
    scene.maxy = rootElement.attribute("maxy").toDouble();
    scene.backgroundcolor =
            StringToColor(rootElement.attribute("backgroundcolor"));

    for(QDomNode n=rootElement.firstChild(); !n.isNull(); n=n.nextSibling())
    {
        QDomElement domElement = n.toElement();

        if (domElement.tagName() == "SceneLayer")
        {
            OGSceneLayer sceneLayer;

            sceneLayer.id = domElement.attribute("id");
            sceneLayer.name = domElement.attribute("name");
            sceneLayer.depth = domElement.attribute("depth").toDouble();
            sceneLayer.position.setX(domElement.attribute("x").toDouble());
            sceneLayer.position.setY(domElement.attribute("y").toDouble());
            sceneLayer.scale.x = domElement.attribute("scalex").toDouble();
            sceneLayer.scale.y = domElement.attribute("scaley").toDouble();
            sceneLayer.rotation =
                    domElement.attribute("rotation").toDouble();

            sceneLayer.alpha = domElement.attribute("alpha").toDouble();
            sceneLayer.colorize =
                    StringToColor(domElement.attribute("colorize"));

            sceneLayer.image = domElement.attribute("image");
            sceneLayer.anim = domElement.attribute("anim");
            sceneLayer.animspeed =
                    domElement.attribute("animspeed").toDouble();

            scene.sceneLayers << sceneLayer;
        }
        else if (domElement.tagName() == "label")
        {
            OGLabel label;

            label.id = domElement.attribute("id");
            label.depth = domElement.attribute("depth").toDouble();
            label.position =
                    StringToPoint(
                        domElement.attribute("x"),
                        domElement.attribute("y"));

            label.align = domElement.attribute("align");
            label.rotation = domElement.attribute("rotation").toDouble();
            label.scale = domElement.attribute("scale").toDouble();
            label.overlay = StringToBool(domElement.attribute("overlay"));
            label.screenspace =
                    StringToBool(domElement.attribute("screenspace"));

            label.font = domElement.attribute("font");
            label.text = domElement.attribute("text");

            scene.labels << label;
        }
    }

    return scene;
}
