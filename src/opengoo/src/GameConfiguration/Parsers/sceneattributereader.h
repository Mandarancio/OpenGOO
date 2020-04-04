#pragma once

#include "wog_scene.h"

template<>
struct AttributeReader<WOGPhysicsObject>
{
    static void Read(const QDomAttr& aAttribute, WOGPhysicsObject* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("static"))
        {
            aOut->dynamic = aAttribute.value() == "false" ? true : false;
        }
        else if (name == QLatin1String("tag"))
        {
            ValueWriter::WriteValue(aOut->tag, aAttribute);
        }
        else if (name == QLatin1String("material"))
        {
            ValueWriter::WriteValue(aOut->material, aAttribute);
        }
        else if (name == QLatin1String("mass"))
        {
            ValueWriter::WriteValue(aOut->mass, aAttribute);
        }
        else if (name == QLatin1String("x"))
        {
            ValueWriter::WriteValue(aOut->x, aAttribute);
        }
        else if (name == QLatin1String("y"))
        {
            ValueWriter::WriteValue(aOut->y, aAttribute);
        }
        else if (name == QLatin1String("rotation"))
        {
            ValueWriter::WriteValue(aOut->rotation, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGImage>
{
    static void Read(const QDomAttr& aAttribute, WOGImage* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("image"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("imagepos"))
        {
            ValueWriter::WriteValue(aOut->imagepos, aAttribute);
        }
        else if (name == QLatin1String("imagescale"))
        {
            ValueWriter::WriteValue(aOut->imagescale, aAttribute);
        }
        else if (name == QLatin1String("imagerot"))
        {
            ValueWriter::WriteValue(aOut->imagerot, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGLine>
{
    static void ReadAnchor(const QDomAttr& aAttribute, WOGLine* aOut)
    {
        auto anchor = ValueWriter::StringToPointF(aAttribute);
        aOut->x = anchor.x();
        aOut->y = anchor.y();
    }

    static void Read(const QDomAttr& aAttribute, WOGLine* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("anchor"))
        {
            ReadAnchor(aAttribute, aOut);
        }
        else if (name == QLatin1String("normal"))
        {
            ValueWriter::WriteValue(aOut->normal, aAttribute);
        }
        else
        {
            ReadAttribute<WOGPhysicsObject>(aAttribute, aOut);
        }
    }
};

template<>
struct AttributeReader<WOGCircle>
{
    static void Read(const QDomAttr& aAttribute, WOGCircle* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("radius"))
        {
            ValueWriter::WriteValue(aOut->radius, aAttribute);
        }
        else if (name == QLatin1String("rotspeed"))
        {
            ValueWriter::WriteValue(aOut->rotspeed, aAttribute);
        }
        else
        {
            ReadAttribute<WOGPhysicsObject>(aAttribute, aOut);
            ReadAttribute(aAttribute, &aOut->image);
        }
    }
};

template<>
struct AttributeReader<WOGRectangle>
{
    static void Read(const QDomAttr& aAttribute, WOGRectangle* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("width"))
        {
            ValueWriter::WriteValue(aOut->width, aAttribute);
        }
        else if (name == QLatin1String("height"))
        {
            ValueWriter::WriteValue(aOut->height, aAttribute);
        }
        else
        {
            ReadAttribute<WOGPhysicsObject>(aAttribute, aOut);
            ReadAttribute(aAttribute, &aOut->image);
        }
    }
};

template<>
struct AttributeReader<WOGCompositeGeom>
{
    static void Read(const QDomAttr& aAttribute, WOGCompositeGeom* aOut)
    {
        ReadAttribute<WOGPhysicsObject>(aAttribute, aOut);
    }
};

template<>
struct AttributeReader<WOGScene::WOGParticle>
{
    static void Read(const QDomAttr& aAttribute, WOGScene::WOGParticle* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("effect"))
        {
            ValueWriter::WriteValue(aOut->effect, aAttribute);
        }
        else if (name == QLatin1String("depth"))
        {
            ValueWriter::WriteValue(aOut->depth, aAttribute);
        }
        else if (name == QLatin1String("pos"))
        {
            ValueWriter::WriteValue(aOut->position, aAttribute);
        }
        else if (name == QLatin1String("pretick"))
        {
            ValueWriter::WriteValue(aOut->pretick, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGVObject>
{
    static void Read(const QDomAttr& aAttribute, WOGVObject* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("depth"))
        {
            ValueWriter::WriteValue(aOut->depth, aAttribute);
        }
        else if (name == QLatin1String("x"))
        {
            ValueWriter::WriteValue(aOut->x, aAttribute);
        }
        else if (name == QLatin1String("y"))
        {
            ValueWriter::WriteValue(aOut->y, aAttribute);
        }
        else if (name == QLatin1String("scalex"))
        {
            ValueWriter::WriteValue(aOut->scalex, aAttribute);
        }
        else if (name == QLatin1String("scaley"))
        {
            ValueWriter::WriteValue(aOut->scaley, aAttribute);
        }
        else if (name == QLatin1String("rotation"))
        {
            ValueWriter::WriteValue(aOut->rotation, aAttribute);
        }
        else if (name == QLatin1String("alpha"))
        {
            ValueWriter::WriteValue(aOut->alpha, aAttribute);
        }
        else if (name == QLatin1String("colorize"))
        {
            ValueWriter::WriteValue(aOut->colorize, aAttribute);
        } else {
            qCritical() << name << aAttribute.value();
        }
    }
};

template<>
struct AttributeReader<WOGSceneLayer>
{
    static void Read(const QDomAttr& aAttribute, WOGSceneLayer* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("name"))
        {
            ValueWriter::WriteValue(aOut->name, aAttribute);
        }
        else if (name == QLatin1String("image"))
        {
            ValueWriter::WriteValue(aOut->image, aAttribute);
        }
        else if (name == QLatin1String("anim"))
        {
            ValueWriter::WriteValue(aOut->anim, aAttribute);
        }
        else if (name == QLatin1String("animspeed"))
        {
            ValueWriter::WriteValue(aOut->animspeed, aAttribute);
        }
        else
        {
            AttributeReader<WOGVObject>::Read(aAttribute, aOut);
        }
    }
};

template<>
struct AttributeReader<WOGButton>
{
    static void Read(const QDomAttr& aAttribute, WOGButton* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("up"))
        {
            ValueWriter::WriteValue(aOut->up, aAttribute);
        }
        else if (name == QLatin1String("over"))
        {
            ValueWriter::WriteValue(aOut->over, aAttribute);
        }
        else if (name == QLatin1String("disabled"))
        {
            ValueWriter::WriteValue(aOut->disabled, aAttribute);
        }
        else if (name == QLatin1String("onclick"))
        {
            ValueWriter::WriteValue(aOut->onclick, aAttribute);
        }
        else if (name == QLatin1String("onmouseenter"))
        {
            ValueWriter::WriteValue(aOut->onmouseenter, aAttribute);
        }
        else if (name == QLatin1String("onmouseexit"))
        {
            ValueWriter::WriteValue(aOut->onmouseexit, aAttribute);
        }
        else if (name == QLatin1String("tooltip"))
        {
            ValueWriter::WriteValue(aOut->tooltip, aAttribute);
        }
        else
        {
            AttributeReader<WOGVObject>::Read(aAttribute, aOut);
        }
    }
};

template<>
struct AttributeReader<WOGButtonGroup>
{
    static void Read(const QDomAttr& aAttribute, WOGButtonGroup* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("osx"))
        {
            ValueWriter::WriteValue(aOut->osx, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGLabel>
{
    static void Read(const QDomAttr& aAttribute, WOGLabel* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("align"))
        {
            ValueWriter::WriteValue(aOut->align, aAttribute);
        }
        else if (name == QLatin1String("overlay"))
        {
            ValueWriter::WriteValue(aOut->overlay, aAttribute);
        }
        else if (name == QLatin1String("screenspace"))
        {
            ValueWriter::WriteValue(aOut->screenspace, aAttribute);
        }
        else if (name == QLatin1String("font"))
        {
            ValueWriter::WriteValue(aOut->font, aAttribute);
        }
        else if (name == QLatin1String("text"))
        {
            ValueWriter::WriteValue(aOut->text, aAttribute);
        }
        else if (name == QLatin1String("scale"))
        {
            ValueWriter::WriteValue(aOut->scalex, aAttribute);
        }
        else
        {
            AttributeReader<WOGVObject>::Read(aAttribute, aOut);
        }
    }
};

template<>
struct AttributeReader<WOGLinearForceField>
{
    static void Read(const QDomAttr& aAttribute, WOGLinearForceField* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("type"))
        {
            ValueWriter::WriteValue(aOut->type, aAttribute);
        }
        else if (name == QLatin1String("force"))
        {
            ValueWriter::WriteValue(aOut->force, aAttribute);
        }
        else if (name == QLatin1String("dampeningfactor"))
        {
            ValueWriter::WriteValue(aOut->dampeningfactor, aAttribute);
        }
        else if (name == QLatin1String("antigrav"))
        {
            ValueWriter::WriteValue(aOut->antigrav, aAttribute);
        }
        else if (name == QLatin1String("geomonly"))
        {
            ValueWriter::WriteValue(aOut->geomonly, aAttribute);
        }
        else if (name == QLatin1String("width"))
        {
            ValueWriter::WriteValue(aOut->width, aAttribute);
        }
        else if (name == QLatin1String("height"))
        {
            ValueWriter::WriteValue(aOut->height, aAttribute);
        }
        else if (name == QLatin1String("center"))
        {
            ValueWriter::WriteValue(aOut->center, aAttribute);
        }
        else if (name == QLatin1String("color"))
        {
            ValueWriter::WriteValue(aOut->color, aAttribute);
        }
        else if (name == QLatin1String("depth"))
        {
            ValueWriter::WriteValue(aOut->depth, aAttribute);
        }
        else if (name == QLatin1String("enabled"))
        {
            ValueWriter::WriteValue(aOut->enabled, aAttribute);
        }
        else if (name == QLatin1String("water"))
        {
            ValueWriter::WriteValue(aOut->water, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGRadialForceField>
{
    static void Read(const QDomAttr& aAttribute, WOGRadialForceField* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("id"))
        {
            ValueWriter::WriteValue(aOut->id, aAttribute);
        }
        else if (name == QLatin1String("type"))
        {
            ValueWriter::WriteValue(aOut->type, aAttribute);
        }
        if (name == QLatin1String("center"))
        {
            ValueWriter::WriteValue(aOut->center, aAttribute);
        }
        else if (name == QLatin1String("radius"))
        {
            ValueWriter::WriteValue(aOut->radius, aAttribute);
        }
        else if (name == QLatin1String("forceatcenter"))
        {
            ValueWriter::WriteValue(aOut->forceatcenter, aAttribute);
        }
        else if (name == QLatin1String("forceatedge"))
        {
            ValueWriter::WriteValue(aOut->forceatedge, aAttribute);
        }
        else if (name == QLatin1String("dampeningfactor"))
        {
            ValueWriter::WriteValue(aOut->dampeningfactor, aAttribute);
        }
        else if (name == QLatin1String("antigrav"))
        {
            ValueWriter::WriteValue(aOut->antigrav, aAttribute);
        }
        else if (name == QLatin1String("geomonly"))
        {
            ValueWriter::WriteValue(aOut->geomonly, aAttribute);
        }
        else if (name == QLatin1String("enabled"))
        {
            ValueWriter::WriteValue(aOut->enabled, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGHinge>
{
    static void Read(const QDomAttr& aAttribute, WOGHinge* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("body1"))
        {
            ValueWriter::WriteValue(aOut->body1, aAttribute);
        }
        if (name == QLatin1String("body2"))
        {
            ValueWriter::WriteValue(aOut->body2, aAttribute);
        }
        else if (name == QLatin1String("anchor"))
        {
            ValueWriter::WriteValue(aOut->anchor, aAttribute);
        }
        else if (name == QLatin1String("lostop"))
        {
            ValueWriter::WriteValue(aOut->lostop, aAttribute);
        }
        else if (name == QLatin1String("histop"))
        {
            ValueWriter::WriteValue(aOut->histop, aAttribute);
        }
        else if (name == QLatin1String("bounce"))
        {
            ValueWriter::WriteValue(aOut->bounce, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGMotor>
{
    static void Read(const QDomAttr& aAttribute, WOGMotor* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("body"))
        {
            ValueWriter::WriteValue(aOut->body, aAttribute);
        }
        else if (name == QLatin1String("speed"))
        {
            ValueWriter::WriteValue(aOut->speed, aAttribute);
        }
        else if (name == QLatin1String("maxforce"))
        {
            ValueWriter::WriteValue(aOut->maxforce, aAttribute);
        }
    }
};

template<>
struct AttributeReader<WOGScene>
{
    static void Read(const QDomAttr& aAttribute, WOGScene* aOut)
    {
        const auto name = aAttribute.name();
        if (name == QLatin1String("minx"))
        {
            ValueWriter::WriteValue(aOut->minx, aAttribute);
        }
        else if (name == QLatin1String("miny"))
        {
            ValueWriter::WriteValue(aOut->miny, aAttribute);
        }
        else if (name == QLatin1String("maxx"))
        {
            ValueWriter::WriteValue(aOut->maxx, aAttribute);
        }
        else if (name == QLatin1String("maxy"))
        {
            ValueWriter::WriteValue(aOut->maxy, aAttribute);
        }
        else if (name == QLatin1String("backgroundcolor"))
        {
            ValueWriter::WriteValue(aOut->backgroundcolor, aAttribute);
        }
    }
};
