#pragma once

#include <QString>
#include <QList>

struct WOGMaterial
{
    QString id;
    float friction;
    float bounce;
    float minbouncevel;
    int stickiness;
};

typedef QList<WOGMaterial> WOGMaterialList;

// TODO Remove
//struct WOGMaterialList
//{
//    QList<WOGMaterial> material;

//    WOGMaterial* GetMaterial(const QString& id);
//};
