#ifndef OG_IBODY_H
#define OG_IBODY_H

#include <OGPhysicsBody>

struct WOGMaterial;
struct WOGPObject;

class QPainter;

class OGIBody : public og::OGPhysicsBody
{
    WOGPObject* data_;
    WOGMaterial* material_;

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

    WOGPObject* GetData() { return data_; }
    WOGMaterial* GetMaterial() { return material_; }

public:
    OGIBody(WOGPObject* data, WOGMaterial* material);
    virtual ~OGIBody() {}       

    bool walkable() const { return walkable_; }

    void SetDebug(bool debug) { debug_ = debug; }

    void Draw(QPainter* p) { _Draw(p); }
};

#endif // OG_IBODY_H
