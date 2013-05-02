#include "exitsensor.h"
#include "circle.h"
#include "opengoo.h"
#include "og_userdata.h"

ExitSensor::ExitSensor(const QString &id, const Circle &circle
                       , const OGSensorFilter &filter)
    : OGCircleSensor(id, circle)
    , balls_(0)
    , isClosed_(true)
{
    SetFilter(filter);
}

void ExitSensor::_BeginContact(Fixture* fixture)
{
    b2Body* body = fixture->GetBody();
    void* userdata = body->GetUserData();

    if (!userdata) return;

    OGUserData* data = OGUserData::GetUserData(userdata);

    if (data)
    {
        if (data->type == OGUserData::BALL)
        {
            OGBall* ball = static_cast<OGBall*>(data->data);

            if (ball->IsAttached())
            {
                balls_++;

                if (balls_ == 1) _OpenPipe();

                data->isTouching = true;

            }
            else if (!isClosed_ && ball->IsSuckable())
            {
                data->isTouching = true;
                ball->SetSuction(true);                
            }
        }
    }
}

void ExitSensor::_EndContact(Fixture* fixture)
{
    void* userdata = fixture->GetBody()->GetUserData();

    if (!userdata) return;

    OGUserData* data = OGUserData::GetUserData(userdata);

    if (data)
    {
        if (data->type == OGUserData::BALL)
        {
            OGBall* ball = static_cast<OGBall*>(data->data);

            if (ball->IsAttached())
            {
                balls_--;

                if (balls_ == 0) _ClosePipe();

                data->isTouching = false;
            }
            else if (ball->IsSuckable())
            {
                ball->SetSuction(false);
            }
        }
    }
}

inline void ExitSensor::_OpenPipe()
{
    OpenGOO::instance()->OpenPipe();
    isClosed_ = false;
}

inline void ExitSensor::_ClosePipe()
{
    OpenGOO::instance()->ClosePipe();
    isClosed_ = true;
}

void ExitSensor::_ApplyForce(OGBall* ball)
{
    QVector2D center = GetPosition();
    QVector2D position = ball->GetCenter();
    QVector2D d = center - position;

    float force = 40.0f;

    if (d.lengthSquared() < FLT_EPSILON * FLT_EPSILON)
    {
        return;
    }

    d.normalize();
    QVector2D F = force * d;

    b2Vec2 F2(F.x(), F.y());
    b2Vec2 p(position.x(), position.y());

    ball->body->SetGravityScale(-2.0f);
}
