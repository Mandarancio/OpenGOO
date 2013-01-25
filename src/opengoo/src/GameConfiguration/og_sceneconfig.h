#ifndef OG_SCENECONFIG_H
#define OG_SCENECONFIG_H

#include "og_xmlconfig.h"
#include "wog_scene.h"

class OGSceneConfig : public OGXmlConfig
{
public:
    OGSceneConfig(const QString & filename);
    WOGScene* Parser();

private:
    WOGButton* CreateButton(const QDomElement & element);
    WOGLabel* CreateLabel(const QDomElement & element);
    WOGSceneLayer* CreateSceneLayer(const QDomElement & element);

    WOGCircle* CreateCircle(const QDomElement & element);
    WOGLine* CreateLine(const QDomElement & element);
    WOGRectangle* CreateRectanle(const QDomElement & element);

    WOGLinearForceField* CreateLinearForceField(const QDomElement & element);
    WOGParticle* CreateParticle(const QDomElement & element);
    WOGButtonGroup* CreateButtonGroup(const QDomElement & element);
    WOGRadialForceField* CreateRadialForceField(const QDomElement & element);

    void CreatePObject(WOGPObject* pobject, const QDomElement & element);
};

#endif // OG_SCENECONFIG_H
