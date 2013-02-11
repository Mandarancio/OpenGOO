#include "wog_material.h"

WOGMaterial* WOGMaterialList::GetMaterial(const QString & id)
{
    Q_FOREACH (WOGMaterial* m, material)
    {
        if (m->id == id) { return m; }
    }

    return 0;
}
