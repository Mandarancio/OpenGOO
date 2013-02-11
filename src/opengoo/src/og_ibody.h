#ifndef OG_IBODY_H
#define OG_IBODY_H

#include "og_physicsbody.h"
#include "wog_pobject.h"
#include "wog_material.h"

#include <QStringList>

class OGIBody : public OGPhysicsBody
{
    WOGPObject* data_;
    WOGMaterial* material_;

    bool walkable_;

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

    WOGPObject* GetData() { return data_; }
    WOGMaterial* GetMaterial() { return material_; }

public:
    OGIBody(WOGPObject* data, WOGMaterial* material);
    virtual ~OGIBody() {}       

    bool walkable() const { return walkable_; }
};

#endif // OG_IBODY_H
