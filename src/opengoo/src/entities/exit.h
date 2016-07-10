#pragma once

#include <memory>

#include <QMap>

#include "GameEngine/entity.h"
#include "PhysicsEngine/og_sensor.h"
#include "og_ball.h"

struct WOGLevelExit;
struct ExitEventListener;

typedef QList<OGBall*> BallList;

class Exit : public og::Entity
{
    struct Impl;
    std::unique_ptr<Impl> _pImpl;
    ExitEventListener* m_listener;
    std::unique_ptr<og::physics::Sensor> m_sensor;
    BallList m_balls;
    int m_ball;
    bool m_isClosed;

    Exit(const Exit&);
    Exit& operator=(const Exit&);

public:
    Exit(const WOGLevelExit& a_exit, ExitEventListener* a_listener);
    ~Exit();

    void Update();

    int GetBalls() const;

    void Close();

    void OnTriggerEnter(Fixture* a_fixture);

    void OnTriggerExit(Fixture* a_fixture);
};
