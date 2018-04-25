#pragma once

#include <QDomElement>

#include "wog_material.h"

// root tag materials
class OGMaterialConfig
{
public:
    static void Parser(const QDomElement& aElement, WOGMaterialList* obj);
};
