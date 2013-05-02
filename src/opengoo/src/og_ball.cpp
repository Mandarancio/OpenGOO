#include "og_ball.h"
#include "wog_ball.h"
#include "wog_level.h"
#include "og_walk.h"
#include "og_climb.h"
#include "og_fly.h"
#include "og_userdata.h"
#include "physics.h"
#include "og_ibody.h"
#include "og_world.h"
#include "opengoo.h"

#include <QLineF>
#include <QPen>
#include <QPainter>

#include <QtCore/qmath.h>

inline float LengthSquared(float x1, float y1, float x2, float y2);
inline float LengthSquared(const QPointF* p1, const QPointF* p2);

float LengthSquared(const QPointF* p1, const QPointF* p2)
{
    return LengthSquared(p1->x(), p1->y(), p2->x(), p2->y());
}

float LengthSquared(float x1, float y1, float x2, float y2)
{
    return qPow((x2 - x1), 2.0f) + qPow((y2 - y1), 2.0f);
}

OGBall::OGBall(WOGBallInstance* data, WOGBall* configuration)
    : pData_(data)
    , pConfig_(configuration)
    , pWalkBehavior_(0)
    , pClimbBehavior_(0)
    , pFlyBehavior_(0)
{
    const float K = 0.1f;

    isAttached_ = false;
    isClimbing_ = false;
    isDetaching_ = false;
    isDragging_ = false;
    isFalling_ = false;
    isMarked_ = false;
    isStanding_ = false;
    isWalking_ = false;    
    isDraggable_ = pConfig_->attribute.player.draggable;
    isSuckable_ = pConfig_->attribute.level.suckable;

    material_.bounce = 0.1f;
    material_.friction = 15.0f;
    numberStrands_ = 0;
    id_ = -1;
    isInit_ = false;
    pTargetBall_ = 0;
    isSuction_ = false;
    isExit_ = false;

    WOGBallShape* ballShape = GetShape();

    // Get position, angle and mass of ball
    float x = pData_->x;
    float y = pData_->y;
    float angle = pData_->angle;
    float mass = pConfig_->attribute.core.mass;
    int v = ballShape->variation * 100; // convert the variation to a percentage

    if (v > 100) { v = 100; }

    OGPhysicsBody* obj = 0;

    if (ballShape->type == "circle")
    {
        type_ = OGBall::C_BALL;
        obj = CreateCircle(x, y, angle, mass, ballShape, v);
    }
    else if (ballShape->type == "rectangle")
    {
        type_ = OGBall::R_BALL;
        obj = CreateReactangle(x, y, angle, mass, ballShape, v);
    }

    if (obj != 0)
    {
        body = obj->body;
        fixture = obj->fixture;
        shape = obj->shape;
    }

    towerMass_ = pConfig_->attribute.core.towermass * K;

    pWalkBehavior_ = GetWalkBehavior();
    pClimbBehavior_ = GetClimbBehavior();
    pFlyBehavior_ = GetFlyBehavior();

    SetWalkSpeed(pConfig_->attribute.movement.walkspeed);
    SetClimbSpeed(pConfig_->attribute.movement.climbspeed);
}

OGBall::~OGBall()
{
    delete pWalkBehavior_;
    delete pClimbBehavior_;
    delete pFlyBehavior_;
}

QVector2D OGBall::GetCenter() const
{
    b2Vec2 pos = body->GetPosition();

    return QVector2D(pos.x, pos.y);
}

OGPhysicsBody* OGBall::CreateCircle(float x, float y, float angle
                                    , float mass, WOGBallShape* shape
                                    , int variation)
{
    WOGCircleBall* obj = static_cast<WOGCircleBall*>(shape);
    float radius = obj->radius;

    if (variation >= 1) { radius += radius * (qrand() % variation) * 0.01f; }

    OGUserData* data = new OGUserData;
    data->type = OGUserData::BALL;
    data->isTouching = false;
    data->data = this;

    return createCircle(x, y, radius, angle, &material_, true, mass, data);
}

OGPhysicsBody* OGBall::CreateReactangle(float x, float y, float angle
                                        , float mass, WOGBallShape* shape
                                        , int variation)
{
    WOGRectangleBall* obj = static_cast<WOGRectangleBall*>(shape);
    float w = obj->width;
    float h = obj->height;

    if (variation >= 1)
    {
        w += w * (qrand() % variation) * 0.01f;
        h += h * (qrand() % variation) * 0.01f;
    }

    return createRectangle(x, y, w, h, angle, &material_, true, mass, 0);
}

inline void OGBall::SetBodyPosition(float x, float y)
{
    setBodyPosition(this, x, y);
}

void OGBall::Detache()
{
    b2JointEdge* joints = body->GetJointList();

    QList<OGStrand*> strands;
    OGUserData* data;

    while (joints)
    {
        data = static_cast<OGUserData*>(joints->joint->GetUserData());
        strands << static_cast<OGStrand*>(data->data);
        joints = joints->next;
    }

    while (!strands.isEmpty()) { _RemoveStrand(strands.takeFirst()); }

    body->SetAwake(false);
}

void OGBall::Attache(OGBall* ball) { _CreateStrand(this, ball); }

void OGBall::Attache()
{
    while (!jointBalls_.isEmpty()) { Attache(jointBalls_.takeFirst()); }
}

void OGBall::SetCurrentPosition(const b2Vec2 &pos)
{
    curPos_.setX(pos.x);
    curPos_.setY(pos.y);
}

void OGBall::SetExit(bool exit)
{
    isExit_ = exit;
    body->SetActive(!exit);    
}

void OGBall::Update()
{   

    SetCurrentPosition(GetBodyPosition());

    b2ContactEdge* edge = body->GetContactList();

    if (!isClimbing_ && !isDragging_)
    {
        if (edge == 0)
        {
            // The ball's falling

            isFalling_ = true;
            isStanding_ = false;
            isWalking_ = false;
        }
        else if (IsOnWalkableGeom(edge))
        {
            isFalling_ = false;

            if (isAttached_)
            {
                // The ball's standing on a walkable geom

                isStanding_ = true;
                isWalking_ = false;
            }
            else if (IsCanClimb())
            {
                // The ball should climbing

                isClimbing_ = true;
                isStanding_ = false;
                isWalking_ = false;
                setMass(this, GetTowerMass());
            }
            else
            {
                // The ball's walking

                isWalking_ = true;
                isStanding_ = false;
            }
        }
        else
        {
            // The ball's standing on not a walkable geom

            isFalling_ = false;
            isStanding_ = true;
            isWalking_ = false;
        }
    }

    if ((isClimbing_ || isWalking_) && !isMarked_) { Move(); }
}

inline void OGBall::Move()
{
    if (isClimbing_)
    {      
        if (IsCanClimb() && !isSuction_)
        {
            Algorithm2();
        }

        if (pTargetBall_->IsAttached())
        {
            SetClimbTarget(GetTarget());
            PerformClimb();
        }
        else { isClimbing_ = false; }
    }
    else if (isWalking_)
    {
        FindTarget();
        SetWalkTarget(GetTarget());
        PerformWalk();
    }
}

inline bool OGBall::IsCanClimb()
{
    b2Vec2 pos(GetTarget()->x(), GetTarget()->y());

    if (fixture->TestPoint(pos) && pTargetBall_->IsAttached())
    {
        return true;
    }
    else { return false; }
}

// TODO need a new algorithm
inline bool OGBall::IsOnWalkableGeom(b2ContactEdge* edge)
{
    OGUserData* data;
    OGIBody* geom;

    while (edge) // collied with geom
    {
        data = static_cast<OGUserData*>(edge->other->GetUserData());

        if (data)
        {
            if (data->type == OGUserData::GEOM)
            {
                geom = static_cast<OGIBody*>(data->data);

                if (geom->walkable()) { return true; }
            }
        }

        edge = edge->next;
    }

    return false;
}

void OGBall::Paint(QPainter* painter, bool debug)
{
    Q_UNUSED(debug)

    if (isExit()) return;

    float posX, posY, radius, angle;
    QLineF line;

    const float K = 10.0f;
    const qreal DEGREE = 57.2957795;

    posX = GetX() * K;
    posY = GetY() * K * (-1.0);
    radius = shape->GetRadius() * K;
    angle = body->GetAngle() * DEGREE;
    line.setP1(QPointF(posX, posY));
    line.setP2(QPointF(posX + radius, posY));
    line.setAngle(angle);

    QPen pen(Qt::yellow,  2.0);

    painter->save();
    painter->setPen(pen);
    painter->drawLine(line);

    if (isAttached_)
    {
        pen.setColor(Qt::blue);
        painter->setPen(pen);
    }
    else if (isClimbing_)
    {
        pen.setColor(Qt::red);
        painter->setPen(pen);
    }
    else if (isWalking_)
    {
        pen.setColor(Qt::black);
        painter->setPen(pen);
    }
    else if (isFalling_)
    {
        pen.setColor(Qt::green);
        painter->setPen(pen);
    }

    if (isStanding_)
    {
        pen.setColor(Qt::yellow);
        painter->setPen(pen);
    }

    if (isMarked_)
    {
        pen.setColor(Qt::magenta);
        painter->setPen(pen);
    }

    painter->drawEllipse(QPointF(posX, posY), radius, radius);
    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->setFont(QFont("Times", 12, QFont::Bold));
    painter->drawText(QPointF(posX, posY), QString::number(id()));

    int x1, y1, x2, y2;

    Q_FOREACH(OGBall * ball, jointBalls_)
    {
        x1 = GetX() * K;
        y1 = GetY() * K * (-1.0);
        x2 = ball->GetX() * K;
        y2 = ball->GetY() * K * (-1.0);
        painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }

    painter->restore();
}

void OGBall::SetSuction(bool suction)
{
    isSuction_ = suction;

    if (suction)
    {
        isClimbing_ = false;
    }
}

inline void OGBall::SetTarget(float x, float y)
{
    target_.setX(x);
    target_.setY(y);
}

inline void OGBall::SetTarget(OGBall* target)
{
    SetTarget(target->GetX(), target->GetY());
}

void OGBall::AddStrand()
{
    numberStrands_++;

    if (!isAttached_)
    {
        isAttached_ = true;

        if (!IsDetachable()) { isDraggable_ = false; }
    }
}

void OGBall::ReleaseStrand()
{
    if (--numberStrands_ == 0)
    {
        isAttached_ = false;
        isDraggable_ = pConfig_->attribute.player.draggable;
    }
}

inline void OGBall::FindJointBalls()
{
    float dist;
    float minlen = pConfig_->stand->minlen;
    float maxlen1 = pConfig_->stand->maxlen1;

    jointBalls_.clear();

    Q_FOREACH(OGBall * ball, _GetWorld()->balls())
    {
        if (ball->IsAttached())
        {
            dist = Distance(ball) * 10;

            if (dist >= minlen && dist <= maxlen1)
            {
                jointBalls_ << ball;

                if (jointBalls_.size() == GetMaxStrands()) { break; }
            }
        }
    }

    if (GetMaxStrands() >= 2 && jointBalls_.size() < 2)
    {
        jointBalls_.clear();
    }
}

void OGBall::FindTarget()
{
    OGBall* nearestBall = 0;
    float dist1, dist2;

    Q_FOREACH(OGBall * ball, _GetWorld()->balls())
    {
        if (ball->IsStanding())
        {
            if (nearestBall == 0)
            {
                nearestBall = ball;
                dist1 = DistanceSquared(ball);
            }
            else
            {
                dist2 = DistanceSquared(ball);

                if (dist2 < dist1)
                {
                    dist1 = dist2;
                    nearestBall = ball;
                }
            }
        }
    }

    if (nearestBall != 0) SetTarget(nearestBall);
    pTargetBall_ = nearestBall;
}

// Pickup ball

void OGBall::MouseDown(const QPoint &pos)
{
    const float K = 0.1f;

    isDragging_ = true;
    isClimbing_ = false;
    isFalling_ = false;
    isStanding_ = false;
    isWalking_ = false;
    float x = pos.x() * K;
    float y = pos.y() * K;

    SetBodyPosition(x, y);
}

void OGBall::MouseUp(const QPoint &pos)
{
    Q_UNUSED(pos)

    if (!isAttached_)
    {
        while (!jointBalls_.isEmpty()) { Attache(jointBalls_.takeFirst()); }
    }

    isDragging_ = false;
    body->SetAwake(true);
}

void OGBall::MouseMove(const QPoint &pos)
{
    const float K = 0.1f;

    float x = pos.x() * K;
    float y = pos.y() * K;

    SetBodyPosition(x, y);

    if (IsAttached()) { Detache(); }
    else if (GetMaxStrands() > 0) { FindJointBalls(); }
}

void OGBall::SetMarked(bool status)
{
    isMarked_ = status;

    if (isMarked_)
    {
        if (isWalking_ || isClimbing_) { body->SetAwake(false); }
    }

}

bool OGBall::TestPoint(const QPoint &pos)
{
    const float K = 0.1f;

    float x = pos.x() * K;
    float y = pos.y() * K;

    return fixture->TestPoint(b2Vec2(x, y));
}

void OGBall::Algorithm2()
{
    if (_GetWorld()->nearestball() == 0) return;

    OGUserData* data;
    float dist1, dist2;

    b2JointEdge* joints = 0;
    joints = pTargetBall_->GetJoints();
    OGBall* ball = 0;
    b2Body* b1 = 0;
    b2Body* b2 = 0;

    b2Vec2 pos(_GetWorld()->nearestball()->GetBodyPosition());

    while (joints)
    {
        if (b1 == 0)
        {
            b1 = joints->other;
            dist1 = b2Distance(b1->GetPosition(), pos);
        }
        else
        {
            b2 = joints->other;
            dist2 = b2Distance(b2->GetPosition(), pos);

            if (dist2 < dist1)
            {
                dist1 = dist2;
                b1 = b2;
            }
        }

        joints = joints->next;
    }

    if (b1 != 0)
    {
        data = static_cast<OGUserData*>(b1->GetUserData());
        ball = static_cast<OGBall*>(data->data);
        SetTarget(ball);
        pTargetBall_ = ball;
    }
}

inline float OGBall::DistanceSquared(OGBall* b1, OGBall* b2) const
{
    QVector2D p1 = b1->GetPosition();
    QVector2D p2 = b2->GetPosition();

    return (p2 - p1).lengthSquared();
}

inline float OGBall::DistanceSquared(OGBall* b)
{
    return DistanceSquared(b, this);
}

inline float OGBall::Distance(OGBall* b1, OGBall* b2) const
{
    QVector2D p1 = b1->GetPosition();
    QVector2D p2 = b2->GetPosition();

    return (p2 - p1).length();
}

inline float OGBall::Distance(OGBall* b)
{
    return Distance(b, this);
}

inline OGWorld* OGBall::_GetWorld()
{
    return OpenGOO::instance()->GetWorld();
}

void OGBall::_RemoveStrand(OGStrand* strand)
{
    OGWorld* world = _GetWorld();
    world->RemoveStrand(strand);
}

void OGBall::_CreateStrand(OGBall* b1, OGBall* b2)
{
    OGWorld* world = _GetWorld();
    world->CreateStrand(b1, b2);
}

OGUserData* OGBall::GetUserData()
{
    return OGUserData::GetUserData(body->GetUserData());
}

inline void OGBall::PerformWalk() { pWalkBehavior_->Walk(); }
inline void OGBall::PerformClimb() { pClimbBehavior_->Climb(); }
inline void OGBall::PerformFly() { pFlyBehavior_->Fly(); }

inline void OGBall::SetWalkTarget(QPointF* pos)
{
    pWalkBehavior_->SetTarget(pos->x(), 0);
}

inline void OGBall::SetClimbTarget(QPointF* pos)
{
    pClimbBehavior_->SetTarget(*pos);
}

inline void OGBall::SetFlyTarget(QPointF* pos)
{
    pFlyBehavior_->SetTarget(*pos);
}

inline void OGBall::SetWalkSpeed(float speed) { pWalkBehavior_->SetSpeed(speed); }
inline void OGBall::SetClimbSpeed(float speed) { pClimbBehavior_->SetSpeed(speed); }

inline float OGBall::GetAngle() const { return pData_->angle; }
inline QString OGBall::GetType() const { return pData_->type; }

inline WOGBallShape* OGBall::GetShape() const
{
    return pConfig_->attribute.core.shape;
}

QString OGBall::GetStrandType() const
{
    return pConfig_->stand->type;
}

inline bool OGBall::IsDetachable() const { return pConfig_->attribute.player.detachable; }

QString OGBall::GetId() const { return pData_->id; }

int OGBall::GetMaxStrands() const { return pConfig_->attribute.core.strands; }

OGIWalkBehavior* OGBall::GetWalkBehavior()
{
    if (!pWalkBehavior_) pWalkBehavior_ = new OGWalk(this);

    return pWalkBehavior_;
}

OGIClimbBehavior* OGBall::GetClimbBehavior()
{
    if (!pClimbBehavior_) pClimbBehavior_ = new OGClimb(this);

    return pClimbBehavior_;
}

OGIFlyBehavior* OGBall::GetFlyBehavior()
{
    if (!pFlyBehavior_) pFlyBehavior_ = new OGFly(this);

    return pFlyBehavior_;
}
