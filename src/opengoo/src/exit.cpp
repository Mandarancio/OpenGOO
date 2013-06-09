#include "exit.h"
#include "wog_exit.h"
#include "exitsensor.h"

#include <OGPhysicsEngine>
#include "circle.h"
#include "physics.h"

#include "opengoo.h"
#include "og_world.h"
#include "og_userdata.h"

using namespace og;

struct Exit::Impl
{
    ExitSensor* pSensor;
    int balls;
    bool isClosed;
};

Exit::Exit(WOGLevelExit* exit) : _pImpl(new Impl)
{       
    // [1] Create sensor
    Circle c = Circle(exit->pos, exit->radius) / 10.0f;
    OGSensorFilter f = {physics::EXIT, physics::BALL};
    _pImpl->pSensor = new ExitSensor(c, f);

    // [2] Add sensor
    PEngine::GetInstance()->AddSensor(_pImpl->pSensor);

    _pImpl->balls = 0;

    _pImpl->isClosed = false;
}

Exit::~Exit()
{
    PEngine::GetInstance()->RemoveSensor(_pImpl->pSensor);

    delete _pImpl->pSensor;
}

void Exit::Update()
{
    if (_pImpl->isClosed) return;

    QVector2D center = _pImpl->pSensor->GetPosition();
    OpenGOO* game = OpenGOO::instance();
    QList<OGBall*> balls = game->GetWorld()->balls();

    Q_FOREACH(OGBall * ball, balls)
    {
        if (ball->isExit()) continue;

        OGUserData* userData = ball->GetUserData();

        if (userData && userData->isTouching)
        {
            QVector2D position = ball->GetCenter();
            QVector2D d = center - position;

            float force = 40.0f;

            if (d.lengthSquared() < FLT_EPSILON * FLT_EPSILON)
            {
                continue;
            }

            d.normalize();

            QVector2D F =  force * d;

            ball->ApplyForce(F, position);
        }

        if (ball->isSuction())
        {
            QVector2D position = ball->GetCenter();
            float lq = (center - position).lengthSquared();

            if (lq <= 1.0f)
            {
                _pImpl->balls++;
                ball->SetExit(true);
            }
        }
    }
}

int Exit::Balls() const { return _pImpl->balls; }

void Exit::Close()
{
    _pImpl->isClosed = true;
    PEngine::GetInstance()->RemoveSensor(_pImpl->pSensor);
    OpenGOO* game = OpenGOO::instance();
    QList<OGBall*> balls = game->GetWorld()->balls();

    Q_FOREACH(OGBall * ball, balls)
    {
        ball->SetSuction(false);
    }

    game->ClosePipe();
}
