#include "og_sceneconfig.h"
#include "wog_scene.h"

struct OGSceneConfigHelper
{
    static WOGButton* CreateButton(const QDomElement& element);
    static WOGLabel* CreateLabel(const QDomElement& element);
    static WOGSceneLayer* CreateSceneLayer(const QDomElement& element);

    static void LoadCircle(WOGCircle& a_obj, const QDomElement& element);
    static WOGLine* CreateLine(const QDomElement& element);
    static void LoadRectanle(WOGRectangle& a_obj, const QDomElement& element);
    static WOGCompositeGeom* CreateCompositeGeom(const QDomElement& el);

    static WOGLinearForceField* CreateLinearForceField(const QDomElement& element);
    static WOGParticle* CreateParticle(const QDomElement& element);
    static WOGButtonGroup* CreateButtonGroup(const QDomElement& element);
    static WOGRadialForceField* CreateRadialForceField(const QDomElement& element);

    static void CreatePObject(WOGPObject* pobject, const QDomElement& element);
};

WOGButton* OGSceneConfigHelper::CreateButton(const QDomElement& element)
{
    WOGButton* obj = new WOGButton;
    obj->id = element.attribute("id");
    obj->depth = element.attribute("depth").toFloat();
    obj->position = OGXmlConfig::StringToPoint(element.attribute("x"), element.attribute("y"));

    obj->scale = OGXmlConfig::StringToPoint(element.attribute("scalex"), element.attribute("scaley"));

    obj->rotation = element.attribute("rotation").toFloat();
    obj->alpha = element.attribute("alpha", "1").toFloat();
    obj->colorize = OGXmlConfig::StringToColor(element.attribute("colorize", "255,255,255"));

    obj->up = element.attribute("up");
    obj->over = element.attribute("over");
    obj->disabled = element.attribute("disabled");
    obj->onclick = element.attribute("onclick");
    obj->onmouseenter = element.attribute("onmouseenter");
    obj->onmouseexit = element.attribute("onmouseexit");
    obj->text = element.attribute("text");
    obj->font = element.attribute("font");

    return obj;
}

WOGLabel* OGSceneConfigHelper::CreateLabel(const QDomElement& element)
{
    WOGLabel* obj = new WOGLabel;
    obj->id = element.attribute("id");
    obj->depth = element.attribute("depth").toDouble();
    obj->position = OGXmlConfig::StringToPoint(element.attribute("x")
                                  , element.attribute("y"));

    obj->align = element.attribute("align");
    obj->rotation = element.attribute("rotation").toDouble();
    obj->scale = element.attribute("scale").toDouble();
    obj->overlay = OGXmlConfig::StringToBool(element.attribute("overlay"));
    obj->screenspace = OGXmlConfig::StringToBool(element.attribute("screenspace"));
    obj->font = element.attribute("font");
    obj->text = element.attribute("text");

    return obj;
}

WOGSceneLayer* OGSceneConfigHelper::CreateSceneLayer(const QDomElement& element)
{
    WOGSceneLayer* obj = new WOGSceneLayer;
    obj->id = element.attribute("id");
    obj->name = element.attribute("name");
    obj->depth = element.attribute("depth").toDouble();
    obj->position = OGXmlConfig::StringToPoint(element.attribute("x")
                                  , element.attribute("y"));

    obj->scale = OGXmlConfig::StringToPoint(element.attribute("scalex")
                               , element.attribute("scaley"));

    obj->rotation = element.attribute("rotation").toDouble();
    obj->alpha = element.attribute("alpha", "1").toDouble();
    obj->colorize = OGXmlConfig::StringToColor(element.attribute("colorize"
                                  , "255,255,255"));

    obj->image = element.attribute("image");
    obj->anim = element.attribute("anim");
    obj->animspeed = element.attribute("animspeed").toDouble();

    return obj;
}

void OGSceneConfigHelper::LoadCircle(WOGCircle& a_obj, const QDomElement& element)
{
    a_obj.position = OGXmlConfig::StringToPoint(element.attribute("x"), element.attribute("y"));
    a_obj.radius = element.attribute("radius").toDouble();

    CreatePObject(&a_obj, element);
}

WOGLinearForceField* OGSceneConfigHelper::CreateLinearForceField(const QDomElement& element)
{
    WOGLinearForceField* obj = new WOGLinearForceField;
    obj->type = element.attribute("type");
    obj->force = OGXmlConfig::StringToPoint(element.attribute("force"));
    obj->dampeningfactor = element.attribute("dampeningfactor").toDouble();
    obj->antigrav = OGXmlConfig::StringToBool(element.attribute("antigrav"));
    obj->geomonly = OGXmlConfig::StringToBool(element.attribute("geomonly"));

    return obj;
}

WOGParticle* OGSceneConfigHelper::CreateParticle(const QDomElement& element)
{
    WOGParticle* obj = new WOGParticle;
    obj->effect = element.attribute("effect");
    obj->depth = element.attribute("depth").toDouble();
    obj->position = OGXmlConfig::StringToPoint(element.attribute("pos"));
    obj->pretick = element.attribute("pretick").toDouble();

    return obj;
}

WOGButtonGroup* OGSceneConfigHelper::CreateButtonGroup(const QDomElement& element)
{
    WOGButtonGroup* obj = new WOGButtonGroup;
    obj->id = element.attribute("id");
    obj->osx = OGXmlConfig::StringToPoint(element.attribute("osx"));

    for (QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        obj->button << CreateButton(n.toElement());
    }

    return obj;
}

WOGLine* OGSceneConfigHelper::CreateLine(const QDomElement& element)
{
    WOGLine* obj = new WOGLine;
    obj->anchor = OGXmlConfig::StringToPoint(element.attribute("anchor"));
    obj->normal = OGXmlConfig::StringToPoint(element.attribute("normal"));

    CreatePObject(obj, element);

    return obj;
}

WOGRadialForceField* OGSceneConfigHelper::CreateRadialForceField(const QDomElement& element)
{
    WOGRadialForceField* obj = new WOGRadialForceField;
    obj->id = element.attribute("id");
    obj->type = element.attribute("type");
    obj->center = OGXmlConfig::StringToPoint(element.attribute("center"));
    obj->radius = element.attribute("radius").toDouble();
    obj->forceatcenter = element.attribute("forceatcenter").toDouble();
    obj->forceatedge = element.attribute("forceatedge").toDouble();
    obj->dampeningfactor = element.attribute("dampeningfactor").toDouble();
    obj->antigrav = OGXmlConfig::StringToBool(element.attribute("antigrav"));
    obj->geomonly = OGXmlConfig::StringToBool(element.attribute("geomonly"));
    obj->enabled = OGXmlConfig::StringToBool(element.attribute("enabled"));

    return obj;
}

void OGSceneConfigHelper::LoadRectanle(WOGRectangle& a_obj, const QDomElement& element)
{
    a_obj.position = OGXmlConfig::StringToPoint(element.attribute("x"), element.attribute("y"));
    a_obj.size = OGXmlConfig::StringToSize(element.attribute("width"), element.attribute("height"));
    a_obj.rotation = element.attribute("rotation").toFloat();

    CreatePObject(&a_obj, element);
}

void OGSceneConfigHelper::CreatePObject(WOGPObject* pobject, const QDomElement& element)
{
    pobject->id = element.attribute("id");

    if (OGXmlConfig::StringToBool(element.attribute("static")))
    {
        pobject->dynamic = false;
    }
    else { pobject->dynamic = true; }

    pobject->tag = element.attribute("tag", "walkable");
    pobject->material = element.attribute("material");
}

WOGCompositeGeom* OGSceneConfigHelper::CreateCompositeGeom(const QDomElement& el)
{
    WOGCompositeGeom* obj = new WOGCompositeGeom;
    obj->position = OGXmlConfig::StringToPoint(el.attribute("x"), el.attribute("y"));
    obj->rotation = el.attribute("rotation").toFloat();
    CreatePObject(obj, el);
    QDomElement de;

    for (QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        de = n.toElement();

        if (de.tagName() == "circle")
        {
            obj->circle.push_back(WOGCircle());
            LoadCircle(obj->circle.back(), de);
        }
        else if (de.tagName() == "rectangle")
        {
            obj->rectangle.push_back(WOGRectangle());
            LoadRectanle(obj->rectangle.back(), de);
        }
    }

    return obj;
}

WOGScene* OGSceneConfig::Parser()
{
    WOGScene* scene = new WOGScene;
    scene->minx = rootElement.attribute("minx").toDouble();
    scene->miny = rootElement.attribute("miny").toDouble();
    scene->maxx = rootElement.attribute("maxx").toDouble();
    scene->maxy = rootElement.attribute("maxy").toDouble();
    scene->backgroundcolor =
        StringToColor(rootElement.attribute("backgroundcolor"));

    QDomNode node = rootElement.firstChild();

    while (!node.isNull())
    {
        QDomElement domElement = node.toElement();

        if (domElement.tagName() == "SceneLayer")
        {
            scene->sceneLayer << OGSceneConfigHelper::CreateSceneLayer(domElement);
        }
        else if (domElement.tagName() == "label")
        {
            scene->label << OGSceneConfigHelper::CreateLabel(domElement);
        }
        else if (domElement.tagName() == "buttongroup")
        {
            scene->buttongroup << OGSceneConfigHelper::CreateButtonGroup(domElement);
        }
        else if (domElement.tagName() == "button")
        {
            scene->button << OGSceneConfigHelper::CreateButton(domElement);
        }
        else if (domElement.tagName() == "radialforcefield")
        {
            scene->radialforcefield << OGSceneConfigHelper::CreateRadialForceField(domElement);
        }
        else if (domElement.tagName() == "linearforcefield")
        {
            scene->linearforcefield << OGSceneConfigHelper::CreateLinearForceField(domElement);
        }
        else if (domElement.tagName() == "particles")
        {
            scene->particle << OGSceneConfigHelper::CreateParticle(domElement);
        }
        else if (domElement.tagName() == "circle")
        {
            scene->circle.push_back(new WOGCircle);
            OGSceneConfigHelper::LoadCircle(*scene->circle.back(), domElement);
        }
        else if (domElement.tagName() == "line")
        {
            scene->line << OGSceneConfigHelper::CreateLine(domElement);
        }
        else if (domElement.tagName() == "rectangle")
        {
            scene->rectangle.push_back(new WOGRectangle);
            OGSceneConfigHelper::LoadRectanle(*scene->rectangle.back(), domElement);
        }
        else if (domElement.tagName() == "compositegeom")
        {
            scene->compositegeom << OGSceneConfigHelper::CreateCompositeGeom(domElement);
        }

        node = node.nextSibling();
    }

    return scene;
}
