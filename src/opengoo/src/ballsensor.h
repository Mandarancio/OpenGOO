#pragma once

#include "PhysicsEngine/og_circlesensor.h"
#include "PhysicsEngine/og_physicsengine.h"

class OGBall;

class BallSensor : public og::physics::CircleSensor
{
public:
    BallSensor(og::physics::PhysicsEngine& a_physicEngine, OGBall* b);
    ~BallSensor();

    void update();

private:
    OGBall* m_ball;
    og::physics::PhysicsEngine& m_physicEngine;

private:
    void BeginContact(Fixture* a_fixture);
    void EndContact(Fixture*) {}
};
