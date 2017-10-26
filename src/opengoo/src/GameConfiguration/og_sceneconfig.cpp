#include "og_sceneconfig.h"
#include "wog_scene.h"

struct OGSceneConfigHelper
{
    static WOGButton CreateButton(const QDomElement& element);
    static WOGLabel CreateLabel(const QDomElement& element);
    static WOGSceneLayer CreateSceneLayer(const QDomElement& element);
    static WOGCircle CreateCircle(const QDomElement& element);
    static WOGLine CreateLine(const QDomElement& element);
    static WOGRectangle CreateRectanle(const QDomElement& element);
    static WOGCompositeGeom CreateCompositeGeom(const QDomElement& el);
    static WOGLinearForceField CreateLinearForceField(const QDomElement& element);
    static WOGScene::WOGParticle CreateParticle(const QDomElement& element);
    static WOGButtonGroup CreateButtonGroup(const QDomElement& element);
    static WOGRadialForceField CreateRadialForceField(const QDomElement& element);

    static void LoadPObject(WOGPObject& pobject, const QDomElement& element);
};

WOGButton OGSceneConfigHelper::CreateButton(const QDomElement& element)
{
    WOGButton obj;
    obj.id = element.attribute("id");
    obj.depth = element.attribute("depth").toFloat();
    obj.position = OGXmlConfig::StringToPointF(element.attribute("x"), element.attribute("y"));
    obj.scale = OGXmlConfig::StringToPointF(element.attribute("scalex", "1"), element.attribute("scaley", "1"));
    obj.rotation = element.attribute("rotation","0").toFloat();
    obj.alpha = element.attribute("alpha", "1").toFloat();
    obj.colorize = OGXmlConfig::StringToColor(element.attribute("colorize", "255,255,255"));
    obj.up = element.attribute("up");
    obj.over = element.attribute("over");
    obj.disabled = element.attribute("disabled");
    obj.onclick = element.attribute("onclick");
    obj.onmouseenter = element.attribute("onmouseenter");
    obj.onmouseexit = element.attribute("onmouseexit");
    obj.text = element.attribute("text");
    obj.font = element.attribute("font");

    return obj;
}

WOGLabel OGSceneConfigHelper::CreateLabel(const QDomElement& element)
{
    WOGLabel obj;
    obj.id = element.attribute("id");
    obj.depth = element.attribute("depth", "0").toDouble();
    obj.position = OGXmlConfig::StringToPointF(element.attribute("x"), element.attribute("y"));
    obj.align = element.attribute("align");
    obj.rotation = element.attribute("rotation", "0").toDouble();
    obj.scale = element.attribute("scale", "1").toDouble();
    obj.overlay = OGXmlConfig::StringToBool(element.attribute("overlay"));
    obj.screenspace = OGXmlConfig::StringToBool(element.attribute("screenspace"));
    obj.font = element.attribute("font");
    obj.text = element.attribute("text");

    return obj;
}

WOGSceneLayer OGSceneConfigHelper::CreateSceneLayer(const QDomElement& element)
{
    WOGSceneLayer obj;
    obj.id = element.attribute("id");
    obj.name = element.attribute("name");
    obj.depth = element.attribute("depth", "0").toDouble();
    obj.position = OGXmlConfig::StringToPointF(element.attribute("x"), element.attribute("y"));
    obj.scale = OGXmlConfig::StringToPointF(element.attribute("scalex", "1"), element.attribute("scaley", "1"));
    obj.rotation = element.attribute("rotation", "0").toDouble();
    obj.alpha = element.attribute("alpha", "1").toDouble();
    obj.colorize = OGXmlConfig::StringToColor(element.attribute("colorize", "255,255,255"));
    obj.image = element.attribute("image");
    obj.anim = element.attribute("anim");
    obj.animspeed = element.attribute("animspeed", "1").toDouble();

    return obj;
}

WOGLinearForceField OGSceneConfigHelper::CreateLinearForceField(const QDomElement& element)
{
    WOGLinearForceField obj;
    obj.type = element.attribute("type");
    obj.force = OGXmlConfig::StringToPointF(element.attribute("force"));
    obj.dampeningfactor = element.attribute("dampeningfactor", "0").toDouble();
    obj.antigrav = OGXmlConfig::StringToBool(element.attribute("antigrav"));
    obj.geomonly = OGXmlConfig::StringToBool(element.attribute("geomonly"));

    return obj;
}

WOGScene::WOGParticle OGSceneConfigHelper::CreateParticle(const QDomElement& element)
{
    WOGScene::WOGParticle obj;
    obj.effect = element.attribute("effect");
    obj.depth = element.attribute("depth", "0").toDouble();
    obj.position = OGXmlConfig::StringToPointF(element.attribute("pos"));
    obj.pretick = element.attribute("pretick", "0").toDouble();

    return obj;
}

WOGButtonGroup OGSceneConfigHelper::CreateButtonGroup(const QDomElement& element)
{
    WOGButtonGroup obj;
    obj.id = element.attribute("id");
    obj.osx = OGXmlConfig::StringToPointF(element.attribute("osx"));

    for (auto n = element.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        obj.button.emplace_back(CreateButton(n.toElement()));
    }

    return obj;
}

WOGLine OGSceneConfigHelper::CreateLine(const QDomElement& element)
{
    WOGLine obj;
    obj.anchor = OGXmlConfig::StringToPointF(element.attribute("anchor"));
    obj.normal = OGXmlConfig::StringToPointF(element.attribute("normal"));

    LoadPObject(obj, element);

    return obj;
}

WOGRadialForceField OGSceneConfigHelper::CreateRadialForceField(const QDomElement& element)
{
    WOGRadialForceField obj;
    obj.id = element.attribute("id");
    obj.type = element.attribute("type");
    obj.center = OGXmlConfig::StringToPointF(element.attribute("center"));
    obj.radius = element.attribute("radius", "0").toDouble();
    obj.forceatcenter = element.attribute("forceatcenter", "0").toDouble();
    obj.forceatedge = element.attribute("forceatedge", "0").toDouble();
    obj.dampeningfactor = element.attribute("dampeningfactor", "0").toDouble();
    obj.antigrav = OGXmlConfig::StringToBool(element.attribute("antigrav"));
    obj.geomonly = OGXmlConfig::StringToBool(element.attribute("geomonly"));
    obj.enabled = OGXmlConfig::StringToBool(element.attribute("enabled"));

    return obj;
}

void OGSceneConfigHelper::LoadPObject(WOGPObject& pobject, const QDomElement& element)
{
    pobject.id = element.attribute("id");
    pobject.dynamic = OGXmlConfig::StringToBool(element.attribute("static")) ? false : true;
    pobject.tag = element.attribute("tag", "walkable");
    pobject.material = element.attribute("material");
}

WOGCompositeGeom OGSceneConfigHelper::CreateCompositeGeom(const QDomElement& el)
{
    WOGCompositeGeom obj;
    obj.position = OGXmlConfig::StringToPointF(el.attribute("x"), el.attribute("y"));
    obj.rotation = el.attribute("rotation", "0").toFloat();
    LoadPObject(obj, el);

    for (QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        auto element = n.toElement();

        if (element.tagName() == "circle")
        {
            obj.circle.emplace_back(CreateCircle(element));
        }
        else if (element.tagName() == "rectangle")
        {
            obj.rectangle.push_back(CreateRectanle(element));
        }
    }

    return obj;
}

WOGCircle OGSceneConfigHelper::CreateCircle(const QDomElement& element)
{
    WOGCircle obj;
    obj.position = OGXmlConfig::StringToPointF(element.attribute("x"), element.attribute("y"));
    obj.radius = element.attribute("radius", "0").toDouble();
    LoadPObject(obj, element);

    return obj;
}

WOGRectangle OGSceneConfigHelper::CreateRectanle(const QDomElement& element)
{
    WOGRectangle obj;
    obj.position = OGXmlConfig::StringToPointF(element.attribute("x"), element.attribute("y"));
    obj.size = OGXmlConfig::StringToSize(element.attribute("width"), element.attribute("height"));
    obj.rotation = element.attribute("rotation", "0").toFloat();
    LoadPObject(obj, element);

    return obj;
}

OGSceneConfig::Type OGSceneConfig::Parser()
{
    auto scene = Type(new WOGScene);
    scene->minx = rootElement.attribute("minx", "0").toDouble();
    scene->miny = rootElement.attribute("miny", "0").toDouble();
    scene->maxx = rootElement.attribute("maxx", "0").toDouble();
    scene->maxy = rootElement.attribute("maxy", "0").toDouble();
    scene->backgroundcolor = StringToColor(rootElement.attribute("backgroundcolor"));

    for (auto node = rootElement.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        auto domElement = node.toElement();

        if (domElement.tagName() == "SceneLayer")
        {
            scene->sceneLayer.emplace_back(OGSceneConfigHelper::CreateSceneLayer(domElement));
        }
        else if (domElement.tagName() == "label")
        {
            scene->label.emplace_back(OGSceneConfigHelper::CreateLabel(domElement));
        }
        else if (domElement.tagName() == "buttongroup")
        {
            scene->buttongroup.emplace_back(OGSceneConfigHelper::CreateButtonGroup(domElement));
        }
        else if (domElement.tagName() == "button")
        {
            scene->button.emplace_back(OGSceneConfigHelper::CreateButton(domElement));
        }
        else if (domElement.tagName() == "radialforcefield")
        {
            scene->radialforcefield.emplace_back(OGSceneConfigHelper::CreateRadialForceField(domElement));
        }
        else if (domElement.tagName() == "linearforcefield")
        {
            scene->linearforcefield.emplace_back(OGSceneConfigHelper::CreateLinearForceField(domElement));
        }
        else if (domElement.tagName() == "particles")
        {
            scene->particle.emplace_back(OGSceneConfigHelper::CreateParticle(domElement));
        }
        else if (domElement.tagName() == "circle")
        {
            scene->circle.emplace_back(OGSceneConfigHelper::CreateCircle(domElement));
        }
        else if (domElement.tagName() == "line")
        {
            scene->line.emplace_back(OGSceneConfigHelper::CreateLine(domElement));
        }
        else if (domElement.tagName() == "rectangle")
        {
            scene->rectangle.emplace_back(OGSceneConfigHelper::CreateRectanle(domElement));
        }
        else if (domElement.tagName() == "compositegeom")
        {
            scene->compositegeom.emplace_back(OGSceneConfigHelper::CreateCompositeGeom(domElement));
        }
    }

    return scene;
}
