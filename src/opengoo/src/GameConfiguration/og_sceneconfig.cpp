#include "og_sceneconfig.h"
#include "wog_scene.h"

#include "scenereader.h"

template<typename T>
static void ReadAttributes(T& dData, const QDomElement& aElement)
{
    auto attributes = aElement.attributes();
    for (int i = 0; i < attributes.count(); ++i)
    {
        ReadAttribute(dData, attributes.item(i).toAttr());
    }
}

template<typename T>
static T Create(const QDomElement& aElement)
{
    T obj;
    ReadAttributes(obj, aElement);
    return obj;
}

struct OGSceneConfigHelper
{
    static WOGButton CreateButton(const QDomElement& element);
    static WOGLabel CreateLabel(const QDomElement& element);
    static WOGSceneLayer CreateSceneLayer(const QDomElement& element);
    static WOGCircle CreateCircle(const QDomElement& element);
    static WOGLine CreateLine(const QDomElement& element);
    static WOGRectangle CreateRectanle(const QDomElement& element);
    static WOGCompositeGeom CreateCompositeGeom(const QDomElement& el);
    static WOGLinearForceField CreateLinearForceField(const QDomElement& aElement);
    static WOGScene::WOGParticle CreateParticle(const QDomElement& element);
    static WOGButtonGroup CreateButtonGroup(const QDomElement& element);
    static WOGRadialForceField CreateRadialForceField(const QDomElement& element);

    static void Load(WOGPObject& pobject, const QDomElement& element);
    static void Load(WOGRectangle::Image& img, const QDomElement& element);
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
    obj.tooltip = element.attribute("tooltip");

    return obj;
}

WOGLabel OGSceneConfigHelper::CreateLabel(const QDomElement& element)
{
    WOGLabel obj;
    obj.id = element.attribute("id");
    obj.depth = element.attribute("depth", "0").toFloat();
    obj.position = OGXmlConfig::StringToPointF(element.attribute("x"), element.attribute("y"));
    obj.align = element.attribute("align");
    obj.rotation = element.attribute("rotation", "0").toFloat();
    obj.scale = element.attribute("scale", "1").toFloat();
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
    obj.depth = element.attribute("depth", "0").toFloat();
    obj.position = OGXmlConfig::StringToPointF(element.attribute("x"), element.attribute("y"));
    obj.scale = OGXmlConfig::StringToPointF(element.attribute("scalex", "1"), element.attribute("scaley", "1"));
    obj.rotation = element.attribute("rotation", "0").toFloat();
    obj.alpha = element.attribute("alpha", "1").toFloat();
    obj.colorize = OGXmlConfig::StringToColor(element.attribute("colorize", "255,255,255"));
    obj.image = element.attribute("image");
    obj.anim = element.attribute("anim");
    obj.animspeed = element.attribute("animspeed", "1").toFloat();

    return obj;
}

WOGScene::WOGParticle OGSceneConfigHelper::CreateParticle(const QDomElement& element)
{
    WOGScene::WOGParticle obj;
    obj.effect = element.attribute("effect");
    obj.depth = element.attribute("depth", "0").toFloat();
    obj.position = OGXmlConfig::StringToPointF(element.attribute("pos"));
    obj.pretick = element.attribute("pretick", "0").toFloat();

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

    Load(obj, element);

    return obj;
}

WOGRadialForceField OGSceneConfigHelper::CreateRadialForceField(const QDomElement& element)
{
    WOGRadialForceField obj;
    obj.id = element.attribute("id");
    obj.type = element.attribute("type");
    obj.center = OGXmlConfig::StringToPointF(element.attribute("center"));
    obj.radius = element.attribute("radius", "0").toFloat();
    obj.forceatcenter = element.attribute("forceatcenter", "0").toFloat();
    obj.forceatedge = element.attribute("forceatedge", "0").toFloat();
    obj.dampeningfactor = element.attribute("dampeningfactor", "0").toFloat();
    obj.antigrav = OGXmlConfig::StringToBool(element.attribute("antigrav"));
    obj.geomonly = OGXmlConfig::StringToBool(element.attribute("geomonly"));
    obj.enabled = OGXmlConfig::StringToBool(element.attribute("enabled"));

    return obj;
}

void OGSceneConfigHelper::Load(WOGPObject& pobject, const QDomElement& element)
{
    pobject.id = element.attribute("id");
    pobject.dynamic = OGXmlConfig::StringToBool(element.attribute("static")) ? false : true;
    pobject.tag = element.attribute("tag", "walkable");
    pobject.material = element.attribute("material");
    pobject.mass = element.attribute("mass", "0").toFloat();
}

void OGSceneConfigHelper::Load(WOGRectangle::Image& img, const QDomElement& element)
{
    img.image = element.attribute("image");
    img.imagepos = OGXmlConfig::StringToPointF(element.attribute("imagepos"));
    img.imagerot = element.attribute("imagerot", "0").toFloat();
    img.imagescale = OGXmlConfig::StringToPointF(element.attribute("imagescale", "0, 0"));
}

WOGCompositeGeom OGSceneConfigHelper::CreateCompositeGeom(const QDomElement& el)
{
    WOGCompositeGeom obj;
    obj.position = OGXmlConfig::StringToPointF(el.attribute("x"), el.attribute("y"));
    obj.rotation = el.attribute("rotation", "0").toFloat();
    Load(obj, el);

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
    obj.radius = element.attribute("radius", "0").toFloat();
    Load(obj, element);

    return obj;
}

WOGRectangle OGSceneConfigHelper::CreateRectanle(const QDomElement& element)
{
    WOGRectangle obj;
    obj.position = OGXmlConfig::StringToPointF(element.attribute("x"), element.attribute("y"));
    obj.size = OGXmlConfig::StringToSize(element.attribute("width"), element.attribute("height"));
    obj.rotation = element.attribute("rotation", "0").toFloat();
    Load(obj, element);
    Load(obj.image, element);

    return obj;
}

OGSceneConfig::Type OGSceneConfig::Parser()
{
    auto scene = Type(new WOGScene);
    scene->minx = rootElement.attribute("minx", "0").toFloat();
    scene->miny = rootElement.attribute("miny", "0").toFloat();
    scene->maxx = rootElement.attribute("maxx", "0").toFloat();
    scene->maxy = rootElement.attribute("maxy", "0").toFloat();
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
            scene->linearforcefield.emplace_back(Create<WOGLinearForceField>(domElement));
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
