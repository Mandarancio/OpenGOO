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
        mShapeType = aShapeType;
        return *this;
    }

    PhysicsBodyBuilder& SetRadius(float aRadius)
    {
        mRadius = aRadius * mEngine->GetRatio();
        return *this;
    }

    PhysicsBodyBuilder& SetSize(float aWidth, float aHeight)
    {
        auto k = 0.5f * mEngine->GetRatio();
        mSize.Set(aWidth * k, aHeight * k);
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
        mLine.setLine(x1, y1, x2, y2);
        return *this;
    }

    std::unique_ptr<PhysicsBody> Build()
    {
        if (mShapeType == Shape::e_circle)
        {
            CircleShapeDef def;
            def.radius = mRadius;
            mBodyDef.fixture.shape = &def;
            return mEngine->CreateBody(mBodyDef);
        }
        if (mShapeType == Shape::e_box)
        {
            BoxShapeDef def;
            def.size = mSize;
            mBodyDef.fixture.shape = &def;
            return mEngine->CreateBody(mBodyDef);
        }
        if (mShapeType == Shape::e_line)
        {
            LineShapeDef def;
            def.x1 = mLine.x1();
            def.y1 = mLine.y1();
            def.x2 = mLine.x2();
            def.y2 = mLine.y2();
            mBodyDef.fixture.shape = &def;
            return mEngine->CreateBody(mBodyDef);
        }

        return mEngine->CreateBody(mBodyDef);;
    }

private:
    PhysicsEngine* mEngine;
    float mRadius;
    ShapeDef::Type mShapeType;
    ShapeDef::Size mSize;
    ShapeDef::Line mLine;
    BodyDef mBodyDef;
};
}
}

