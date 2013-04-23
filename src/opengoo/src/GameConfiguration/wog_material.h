#ifndef WOG_MATERIAL_H
#define WOG_MATERIAL_H

#include <QList>
#include <QString>

struct WOGMaterial
{
    QString id;
    float friction;
    float bounce;
    float minbouncevel;
    int stickiness;
};

struct WOGMaterialList
{
    QList<WOGMaterial*> material;

    ~WOGMaterialList()
    {
        while (!material.isEmpty()) { delete material.takeFirst(); }
    }

    WOGMaterial* GetMaterial(const QString & id);
};

#endif // WOG_MATERIAL_H
