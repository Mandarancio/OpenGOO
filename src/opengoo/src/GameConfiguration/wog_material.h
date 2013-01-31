#ifndef WOG_MATERIAL_H
#define WOG_MATERIAL_H

#include <QList>
#include <QString>

#include <QDebug>

struct WOGMaterial
{
    QString id;
    qreal friction;
    qreal bounce;
    qreal minbouncevel;
    int stickiness;

    ~WOGMaterial()
    {
        qDebug("WOGMaterial: End");
    }
};

struct WOGMaterialList
{
    QList<WOGMaterial*> material;

    ~WOGMaterialList()
    {
        while (!material.isEmpty()) { delete material.takeFirst(); }

        qDebug("WOGMaterialList: End");
    }

    WOGMaterial* GetMaterial(const QString & id);
};

#endif // WOG_MATERIAL_H
