#pragma once

#include "GameEngine/entity.h"
#include "PhysicsEngine/joint.h"

#include "og_sprite.h"

class Ball;

class Strand : public og::Entity
{
public:
    Strand(std::shared_ptr<OGSprite> a_sprite, std::unique_ptr<og::physics::Joint> aJoint)
        : Entity(0.0f, 0.0f, a_sprite)
        , mJoint(aJoint.release())
        , mLength(0)
        , mAngle(0)
    {
    }

    Ball* GetBallA() const;

    Ball* GetBallB() const;

private:
    void Render(QPainter& a_painter);

    void LastUpdate();

private:
    std::unique_ptr<og::physics::Joint> mJoint;
    float  mLength;
    float mAngle;
};
