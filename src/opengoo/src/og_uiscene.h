#ifndef OG_UISCENE_H
#define OG_UISCENE_H

#include "og_ui.h"

class OGUIScene
{
    static OGUIButton* _CreateButton(WOGButton* btn, const WOGResources* resrc);
    static QString _GetImgPath(const QString id, const WOGResources* resrc);

public:
    static OGUI* CreateUI(const QString& name);
};

#endif // OG_UISCENE_H
