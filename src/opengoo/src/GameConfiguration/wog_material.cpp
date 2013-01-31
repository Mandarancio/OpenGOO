#include "wog_material.h"

WOGMaterial* WOGMaterialList::GetMaterial(const QString & id)
{
    for (int i=0; i < material.size(); i++)
    {
        if (material.at(i)->id == id)
        {
            return material.at(i);
        }
    }

    return 0;
}
