#pragma once

#include <OGPhysicsBody>

struct WOGMaterial;
struct WOGPObject;

class QPainter;

class OGIBody : public og::PhysicsBody
{
    const WOGPObject& data_;
    const WOGMaterial& material_;

    bool walkable_;    

    virtual void _Draw(QPainter* p) { Q_UNUSED(p) }

protected:
    // S_ - static D - dynamic
    enum OGBodyType
    {
        S_LINE
        , D_LINE
        , S_CIRCLE
        , D_CIRCLE
        , S_RECTANGLE
        , D_RECTANGLE
    };

    OGBodyType m_type;
    bool debug_;

    const WOGPObject& GetData() const { return data_; }
    const WOGMaterial& GetMaterial() const { return material_; }

public:
    OGIBody(const WOGPObject& data, const WOGMaterial& material);
    virtual ~OGIBody() {}       

    bool walkable() const { return walkable_; }

    void SetDebug(bool debug) { debug_ = debug; }

    void Draw(QPainter* p) { _Draw(p); }
};
