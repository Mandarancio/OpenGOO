#pragma once

#include <memory>

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

struct WOGMaterialList : public QList<WOGMaterial>
{
    static std::unique_ptr<WOGMaterialList> Create()
    {
        return std::unique_ptr<WOGMaterialList>(new WOGMaterialList);
    }
};
