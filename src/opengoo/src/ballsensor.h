#pragma once
#include "PhysicsEngine/og_circlesensor.h"

class OGBall;

class BallSensor : public og::OGCircleSensor
{
public:
    BallSensor(OGBall* b);
    ~BallSensor();

    void update();

private:
    OGBall* m_ball;

private:
    void _BeginContact(Fixture* fixture);
    void _EndContact(Fixture*) {}
};
