#pragma once

#include <memory>

#include <QMap>

#include "GameEngine/entity.h"
#include "PhysicsEngine/og_sensor.h"
#include "og_ball.h"


struct WOGLevelExit;
struct ExitEventListener;

class Exit : public og::Entity
{
    typedef void(Exit::*ExitMemFn)(OGUserData&, OGBall*);
    typedef QList<OGBall*> BallList;
    typedef std::unique_ptr<og::physics::Sensor> SensorPtr;

    ExitEventListener* m_listener;
    SensorPtr m_sensor;
    BallList m_balls;
    BallList m_remove;
    bool m_isClosed;

    Exit(const Exit&);
    Exit& operator=(const Exit&);

    void Update();

    void AddBall(OGUserData& a_data, OGBall* a_ball);

    void RemoveBall(OGUserData& a_data, OGBall* a_ball);

    void ProcessFixture(Fixture* a_fixture, ExitMemFn a_callback);

    void OnTriggerEnter(Fixture* a_fixture)
    {
        ProcessFixture(a_fixture, &Exit::AddBall);
    }

    void OnTriggerExit(Fixture* a_fixture)
    {
        ProcessFixture(a_fixture, &Exit::RemoveBall);
    }

    OGBall* GetBall(int a_i)
    {
        assert(a_i >= 0 && a_i < m_balls.size());
        return m_balls[a_i];
    }

public:
    Exit(const WOGLevelExit& a_exit, ExitEventListener* a_listener);

    ~Exit();
};
