#include "exit.h"
#include "wog_exit.h"
#include "exitsensor.h"

#include <OGPhysicsEngine>
#include "circle.h"
#include "physics.h"

#include "opengoo.h"
#include "og_world.h"
#include "og_userdata.h"

struct ExitImpl
{
    ExitSensor* pSensor;
    int balls;
    bool isClosed;
};

Exit::Exit(WOGLevelExit* exit)
{
    // [0] Create pimpl
    pImpl_ = new ExitImpl;

    // [1] Create sensor
    Circle c = Circle(exit->pos, exit->radius) / 10.0f;
    OGSensorFilter f = {physics::EXIT, physics::BALL};
    pImpl_->pSensor = new ExitSensor("levelExit", c, f);

    // [2] Add sensor
    PEngine::GetInstance()->AddSensor(pImpl_->pSensor);

    pImpl_->balls = 0;

    pImpl_->isClosed = false;
}

Exit::~Exit()
{
    PEngine::GetInstance()->RemoveSensor(pImpl_->pSensor->id());

    delete pImpl_->pSensor;
    delete pImpl_;
}

void Exit::Update()
{
    if (pImpl_->isClosed) return;

    QVector2D center = pImpl_->pSensor->GetPosition();
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
                pImpl_->balls++;
                ball->SetExit(true);
            }
        }
    }
}

int Exit::Balls() const { return pImpl_->balls; }

void Exit::Close()
{
    pImpl_->isClosed = true;
    PEngine::GetInstance()->RemoveSensor(pImpl_->pSensor->id());
    OpenGOO* game = OpenGOO::instance();
    QList<OGBall*> balls = game->GetWorld()->balls();

    Q_FOREACH(OGBall * ball, balls)
    {
        ball->SetSuction(false);
    }

    game->ClosePipe();
}
