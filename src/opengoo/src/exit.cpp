#include "exit.h"
#include "wog_exit.h"
#include "exitsensor.h"

#include <OGPhysicsEngine>
#include "circle.h"
#include "physics.h"

#include "opengoo.h"
#include "og_world.h"
#include "og_userdata.h"

#include <QPainter>

struct ExitImpl
{
    ExitSensor* pSensor;
    int balls;
};

Exit::Exit(WOGLevelExit* exit)
{
    pImpl_ = new ExitImpl;
    Circle c = Circle(exit->pos, exit->radius) / 10.0f;
    pImpl_->pSensor = new ExitSensor("exit", c);
    pImpl_->pSensor->SetFilter(physics::EXIT, physics::BALL);
    PEngine::GetInstance()->AddSensor(pImpl_->pSensor);
}

Exit::~Exit()
{
    PEngine::GetInstance()->RemoveSensor("exit");
    delete pImpl_->pSensor;
    delete pImpl_;
}

void Exit::Update()
{
    QVector2D center = pImpl_->pSensor->GetPosition();
    OpenGOO* game = OpenGOO::instance();
    QList<OGBall*> balls = game->GetWorld()->balls();

    Q_FOREACH(OGBall * ball, balls)
    {
        OGUserData* userData = ball->GetUserData();

        if (!userData || !userData->isTouching) continue;

        QVector2D position = ball->GetCenter();
        QVector2D d = center - position;

        float force = 100.0f;

        if (d.lengthSquared() < FLT_EPSILON * FLT_EPSILON)
        {
            continue;
        }

        d.normalize();

        QVector2D F =  force * d;

        ball->ApplyForce(F, position);;
    }
}

void Exit::Painter(QPainter* painter)
{
    painter->save();
    painter->setPen(Qt::white);
    painter->setFont(QFont("Verdana", 12.0, QFont::Bold));
    painter->drawText(120, 20, QString::number(pImpl_->balls));
    painter->restore();
}
