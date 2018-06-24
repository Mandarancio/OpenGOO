#pragma once

#include "geom.h"

class CompositeGeom : public Geom
{
public:
    CompositeGeom(float aX, float aY, const QString& aTag)
        : Geom(aX, aY, aTag)
    {
    }

    std::shared_ptr<Geom>& AddGeom(std::shared_ptr<Geom> aGeom)
    {
        mGeom.push_back(std::move(aGeom));
        return mGeom.back();
    }

private:
    void Update()
    {
        for (auto it = mGeom.begin(); it != mGeom.end(); ++it)
        {
            auto x = GetX() + (*it)->GetX();
            auto y = GetY() + (*it)->GetY();
            (*it)->SetPosition(x, y);
        }
    }

    void Render(QPainter &a_painter)
    {
        for (auto it = mGeom.begin(); it != mGeom.end(); ++it)
        {
            (*it)->Render(a_painter);
        }
    }

private:
    std::vector<std::shared_ptr<Geom>> mGeom;
};

