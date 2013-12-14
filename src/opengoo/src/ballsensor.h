#ifndef BALLSENSOR_H
#define BALLSENSOR_H

#include "PhysicsEngine/og_circlesensor.h"

class OGBall;

class BallSensor : public og::OGCircleSensor
{
public:
    BallSensor(OGBall* b);
    ~BallSensor();

    void update();

private:
    OGBall* _ball;

private:
    void _BeginContact(Fixture *fixture);
    void _EndContact(Fixture *fixture) {}
};

#endif // BALLSENSOR_H
