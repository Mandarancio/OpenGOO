#pragma once
#include "PhysicsEngine/og_circlesensor.h"

class OGBall;

class BallSensor : public og::physics::CircleSensor
{
public:
    BallSensor(OGBall* b);
    ~BallSensor();

    void update();

private:
    OGBall* m_ball;

private:
    void BeginContact(Fixture* a_fixture);
    void EndContact(Fixture*) {}
};
