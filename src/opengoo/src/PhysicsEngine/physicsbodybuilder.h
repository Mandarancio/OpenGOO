#pragma once

#include <QLineF>
#include <QSizeF>

#include "og_physicsengine.h"
#include "og_physicsbody.h"
#include "shapefactory.h"
#include "fixturebuilder.h"

namespace og
{
namespace physics
{
struct PhysicsBodyBuilder
{
    PhysicsBodyBuilder(PhysicsEngine* aEngine)
        : mEngine(aEngine)
    {
    }

    PhysicsBodyBuilder& SetBodyType(BodyDef::Type aType)
    {
        mBodyDef.type = aType;
        return *this;
    }

    PhysicsBodyBuilder& SetPosition(float aX, float aY)
    {
        mBodyDef.position.Set(aX, aY);
        mBodyDef.position *= mEngine->GetRatio();
        return *this;
    }

    PhysicsBodyBuilder& SetPosition(const QPointF& aPos)
    {
        return SetPosition(aPos.x(), aPos.y());
    }

    PhysicsBodyBuilder& SetAngle(float aAngle)
    {
        mBodyDef.angle = aAngle;
        return *this;
    }

    PhysicsBodyBuilder& SetShapeType(Shape::Type aShapeType)
    {
        mBodyDef.shape.type = aShapeType;
        return *this;
    }

    PhysicsBodyBuilder& SetRadius(float aRadius)
    {
        mBodyDef.shape.radius = aRadius * mEngine->GetRatio();
        return *this;
    }

    PhysicsBodyBuilder& SetSize(float aWidth, float aHeight)
    {
        auto k = 0.5f * mEngine->GetRatio();
        mBodyDef.shape.size.Set(aWidth * k, aHeight * k);
        return *this;
    }

    PhysicsBodyBuilder& SetSize(const QSizeF& aSize)
    {
        return SetSize(aSize.width(), aSize.height());
    }

    PhysicsBodyBuilder& SetLine(float aX1, float aY1, float aX2, float aY2)
    {
        auto x1 = aX1 * mEngine->GetRatio();
        auto y1 = aY1 * mEngine->GetRatio();
        auto x2 = aX2 * mEngine->GetRatio();
        auto y2 = aY2 * mEngine->GetRatio();
        mBodyDef.shape.line.setLine(x1, y1, x2, y2);
        return *this;
    }

    std::unique_ptr<PhysicsBody> Build()
    {
        return mEngine->CreateBody(mBodyDef);
    }

private:
    PhysicsEngine* mEngine;
    BodyDef mBodyDef;
};
}
}

