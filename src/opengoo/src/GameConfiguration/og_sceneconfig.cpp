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
            scene.sceneLayer << CreateSceneLayer(domElement);
        }
        else if (domElement.tagName() == "label")
        {            
            scene.label << CreateLabel(domElement);
        }
        else if (domElement.tagName() == "buttongroup")
        {
            OGButtonGroup btnGroup;

            btnGroup.id = domElement.attribute("id");
            btnGroup.osx = StringToPoint(domElement.attribute("osx"));

            QDomNode btnNode = domElement.firstChild();

            for(; !btnNode.isNull(); btnNode=btnNode.nextSibling())
            {
                btnGroup.button << CreateButton(btnNode.toElement());
            }

            scene.buttongroup << btnGroup;
        }
        else if (domElement.tagName() == "button")
        {
            scene.button << CreateButton(domElement);
        }
        else if (domElement.tagName() == "radialforcefield")
        {

        }
        else if (domElement.tagName() == "linearforcefield")
        {

        }
        else if (domElement.tagName() == "particles")
        {

        }
        else if (domElement.tagName() == "circle")
        {

        }
        else if (domElement.tagName() == "line")
        {

        }
    }

    return scene;
}

OGButton OGSceneConfig::CreateButton(const QDomElement & element)
{
    OGButton button = {
        element.attribute("id"),
        element.attribute("depth").toDouble(),
        StringToPoint(
        element.attribute("x"),
        element.attribute("y")
        ),

        StringToPoint(
        element.attribute("scalex"),
        element.attribute("scaley")
        ),

        element.attribute("rotation").toDouble(),
        element.attribute("alpha").toDouble(),
        StringToColor(element.attribute("colorize")),
        element.attribute("up"),
        element.attribute("over"),
        element.attribute("disabled"),
        element.attribute("onclick"),
        element.attribute("onmouseenter"),
        element.attribute("onmouseexit")
    };

    return button;
}

OGLabel OGSceneConfig::CreateLabel(const QDomElement & element)
{
    OGLabel label = {
        element.attribute("id"),
        element.attribute("depth").toDouble(),
        // position
        StringToPoint(
        element.attribute("x"),
        element.attribute("y")
        ),

        element.attribute("align"),
        element.attribute("rotation").toDouble(),
        element.attribute("scale").toDouble(),
        StringToBool(element.attribute("overlay")),
        StringToBool(element.attribute("screenspace")),
        element.attribute("font"),
        element.attribute("text")
    };

    return label;
}

OGSceneLayer OGSceneConfig::CreateSceneLayer(const QDomElement & element)
{
    OGSceneLayer sceneLayer = {
        element.attribute("id"),
        element.attribute("name"),
        element.attribute("depth").toDouble(),
        // position;
        StringToPoint(
        element.attribute("x"),
        element.attribute("y")
        ),
        // scale
        StringToPoint(
        element.attribute("scalex"),
        element.attribute("scaley")
        ),

        element.attribute("rotation").toDouble(),
        element.attribute("alpha").toDouble(),
        StringToColor(element.attribute("colorize")),
        element.attribute("image"),
        element.attribute("anim"),
        element.attribute("animspeed").toDouble()
    };

    return sceneLayer;
}
