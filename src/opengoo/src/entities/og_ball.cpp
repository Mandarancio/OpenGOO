#include <QLineF>
#include <QPen>
#include <QPainter>

#include <QtCore/qmath.h>

#include "GameEngine/input.h"

#include "physics.h"
#include "opengoo.h"
#include "ballsensor.h"

#include "wog_ball.h"
#include "wog_level.h"

#include "og_walk.h"
#include "og_climb.h"
#include "og_fly.h"
#include "og_userdata.h"
#include "og_ibody.h"
#include "og_world.h"
#include "og_ball.h"
#include "og_windowcamera.h"

using namespace og;

//inline float LengthSquared(float x1, float y1, float x2, float y2);
//inline float LengthSquared(const QPointF* p1, const QPointF* p2);

inline static float LengthSquared(float x1, float y1, float x2, float y2)
{
    return qPow((x2 - x1), 2.0f) + qPow((y2 - y1), 2.0f);
}

inline static float LengthSquared(const QPointF* p1, const QPointF* p2)
{
    return LengthSquared(p1->x(), p1->y(), p2->x(), p2->y());
}

OGBall::OGBall(const WOGBallInstance* a_data, const WOGBall* a_conf)
    : Entity(a_data->x, a_data->y)
    , m_data(a_data)
    , m_config(a_conf)
    , pWalkBehavior_(nullptr)
    , pClimbBehavior_(nullptr)
    , pFlyBehavior_(nullptr)
    , m_name(a_data->type)
{
    isAttached_ = false;
    isClimbing_ = false;
    isDetaching_ = false;
    isDragging_ = false;
    isFalling_ = false;
    isMarked_ = false;
    isStanding_ = false;
    isWalking_ = false;
    const auto& attribute = m_config->attribute;
    isDraggable_ = attribute.player.draggable;
    isSuckable_ = attribute.level.suckable;

    material_.bounce = 0.1f;
    material_.friction = 15.0f;
    numberStrands_ = 0;
    id_ = -1;
    isInit_ = false;
    pTargetBall_ = 0;
    pOriginBall_ = 0;
    isSuction_ = false;
    isExit_ = false;

    _isTouching = false;

    WOGBallShape* ballShape = GetShape();

    // Get position, angle and mass of ball
    float x = m_data->x;
    float y = m_data->y;
    float angle = m_data->angle;
    float mass = attribute.core.mass;
    int v = ballShape->variation * 100; // convert the variation to a percentage

    if (v > 100)
        v = 100;

    switch (ballShape->type)
    {
    case WOGBallShape::e_circle:
        type_ = OGBall::C_BALL;
        m_body.reset(CreateCircle(x, y, angle, mass, static_cast<WOGCircleBall&>(*ballShape), v));
        break;
    case WOGBallShape::e_rectangle:
        type_ = OGBall::R_BALL;
        m_body.reset(CreateReactangle(x, y, angle, mass, ballShape, v));
        break;
    }

    towerMass_ = attribute.core.towermass * PhysicsFactory::PixelsToMeters;

    pWalkBehavior_ = GetWalkBehavior();
    pClimbBehavior_ = GetClimbBehavior();
    pFlyBehavior_ = GetFlyBehavior();

    SetWalkSpeed(attribute.movement.walkspeed);
    SetClimbSpeed(attribute.movement.climbspeed);

    if (m_data->discovered)
        _isSleeping = false;
    else
    {
        m_body->body->SetFixedRotation(true);
        _sensor = getSensor();
        _isSleeping = true;
    }
}

OGBall::~OGBall()
{
    delete pWalkBehavior_;
    delete pClimbBehavior_;
    delete pFlyBehavior_;
}

const QVector2D OGBall::GetPhyPosition() const
{
    auto pos = GetBody()->body->GetPosition();

    return QVector2D(pos.x, pos.y);
}

PhysicsBody* OGBall::CreateCircle(float x, float y, float angle, float mass, const WOGCircleBall& shape, int variation)
{
    float radius = shape.radius;

    if (variation >= 1)
        radius += radius * (qrand() % variation) * 0.01f;

    auto data = new OGUserData;
    data->type = OGUserData::BALL;
    data->isTouching = false;
    data->isAttachedOnEnter = false;
    data->data = this;

    return PhysicsFactory::createCircle(x, y, radius, angle, &material_, true, mass, data);
}

PhysicsBody* OGBall::CreateReactangle(float x, float y, float angle
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

    return PhysicsFactory::createRectangle(x, y, w, h, angle, &material_, true, mass, 0);
}

inline void OGBall::SetBodyPosition(float x, float y)
{
    setBodyPosition(GetBody(), x, y);
}

void OGBall::Detache()
{
    b2JointEdge* joints = GetBody()->body->GetJointList();

    QList<OGStrand*> strands;
    OGUserData* data;

    while (joints)
    {
        data = static_cast<OGUserData*>(joints->joint->GetUserData());
        strands << static_cast<OGStrand*>(data->data);
        joints = joints->next;
    }

    while (!strands.isEmpty())
    {
        _RemoveStrand(strands.takeFirst());
    }

    GetBody()->body->SetAwake(false);
}

void OGBall::Attache(OGBall* ball)
{
    _CreateStrand(this, ball);
}

void OGBall::Attache()
{
    while (!jointBalls_.isEmpty())
    {
        Attache(jointBalls_.takeFirst());
    }
}

void OGBall::SetCurrentPosition(const QVector2D& a_pos)
{
    curPos_ = a_pos.toPointF();
}

void OGBall::SetExit(bool exit)
{
    isExit_ = exit;
    GetBody()->body->SetActive(!exit);
}

void OGBall::Update()
{
    if (GetBody()->IsActive())
    {
        auto pos = GetBody()->GetPosition() * PhysicsFactory::MetersToPixels;
        SetPosition(pos);
    }
    else if (isDragging_)
    {
        auto pos = OGWindowCamera::instance()->windowToLogical(og::MouseInput::GetPosition());
        SetPosition(QVector2D(pos));
    }
}

void OGBall::OldUpdate()
{   
    if (_isSleeping)
    {
        if (_isTouching)
        {
            _sensor.reset();
            GetBody()->body->SetFixedRotation(false);
            _isSleeping = false;
            _isTouching = false;
        }
        else
        {
            _sensor->update();

            return;
        }
    }

    SetCurrentPosition(GetPhyPosition());

    b2ContactEdge* edge = GetBody()->body->GetContactList();

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
                setMass(GetBody(), GetTowerMass());
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

    if ((isClimbing_ || isWalking_) && !isMarked_)
        Move();
}

inline void OGBall::Move()
{
    if (isClimbing_)
    {
        if (IsCanClimb() && !isSuction_)
            Algorithm2();

        if (pTargetBall_ && pTargetBall_->IsAttached() &&
                pOriginBall_ && pOriginBall_->IsAttached())
        {
            SetTarget(pTargetBall_);
            SetOrigin(pOriginBall_);
            SetClimbOrigin(GetOrigin());
            SetClimbTarget(GetTarget());
            PerformClimb();
        }
        else
            isClimbing_ = false;
    }
    else if (isWalking_)
    {
        FindTarget();

        if (pTargetBall_)
        {
            SetWalkTarget(GetTarget());
            PerformWalk();
        } //What should they do when no target found?
    }
}

inline bool OGBall::IsCanClimb()
{
    b2Vec2 pos(GetTarget()->x(), GetTarget()->y());

    if (GetBody()->fixture->TestPoint(pos) && pTargetBall_ && pTargetBall_->IsAttached())
        return true;
    else
        return false;
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

                if (geom->walkable())
                    return true;
            }
        }

        edge = edge->next;
    }

    return false;
}

void OGBall::Render(QPainter& a_painter)
{
    Paint(&a_painter, false);
}

void OGBall::Paint(QPainter* painter, bool debug)
{
    Q_UNUSED(debug)

    if (isExit())
        return;

    float posX, posY, radius, angle;
    QLineF line;

    static const qreal DEGREE = 57.2957795;

    auto p = GetPosition();
    posX = p.x();
    posY = -p.y();

    if (GetBody()->IsActive())
    {
        radius = GetPhyRadius() * PhysicsFactory::MetersToPixels;
        angle = GetPhyAngle() * DEGREE;
        line.setP1(QPointF(posX, posY));
        line.setP2(QPointF(posX + radius, posY));
        line.setAngle(angle);
    }
    else
    {
        radius = GetPhyRadius() * PhysicsFactory::MetersToPixels;
        angle = 0;
    }

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

//    auto pos1 = GetPhyPosition() * PhysicsFactory::MetersToPixels;
//    QPointF p1(pos1.x(), -pos1.y());

//    Q_FOREACH(OGBall * ball, jointBalls_)
//    {

//        auto pos2 = ball->GetPhyPosition() * PhysicsFactory::MetersToPixels;
//        QPointF p2(pos2.x(), -pos2.y());
//        painter->drawLine(p1, p2);
//    }

    painter->restore();
}

void OGBall::SetSuction(bool suction)
{
    isSuction_ = suction;

    if (suction)
        isClimbing_ = false;
}

inline void OGBall::SetTarget(float x, float y)
{
    target_.setX(x);
    target_.setY(y);
}

inline void OGBall::SetTarget(OGBall* target)
{
    target_ = target->GetPhyPosition().toPointF();
}

inline void OGBall::SetOrigin(float x, float y)
{
    origin_.setX(x);
    origin_.setY(y);
}

inline void OGBall::SetOrigin(OGBall* target)
{
    origin_ = target->GetPhyPosition().toPointF();
}


void OGBall::AddStrand()
{
    numberStrands_++;

    if (!isAttached_)
    {
        isAttached_ = true;

        if (!IsDetachable())
            isDraggable_ = false;
    }
}

void OGBall::ReleaseStrand()
{
    if (--numberStrands_ == 0)
    {
        isAttached_ = false;
        isDraggable_ = m_config->attribute.player.draggable;
    }
}

inline void OGBall::FindJointBalls()
{
    float dist;
    const auto& strand = m_config->strand;
    float minlen = strand->minlen;
    float maxlen1 = strand->maxlen1;

    jointBalls_.clear();

    Q_FOREACH(OGBall * ball, _GetWorld()->balls())
    {
        if (ball->IsAttached())
        {
            dist = Distance(ball) * PhysicsFactory::MetersToPixels;

            if (dist >= minlen && dist <= maxlen1)
            {
                jointBalls_ << ball;

                if (jointBalls_.size() == GetMaxStrands())
                    break;
            }
        }
    }

    if (GetMaxStrands() >= 2 && jointBalls_.size() < 2)
        jointBalls_.clear();
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

    if (nearestBall != 0)
        SetTarget(nearestBall);

    pTargetBall_ = nearestBall;
}

// Pickup ball

void OGBall::MouseDown(const QPoint &pos)
{
//    if (_isSleeping)
//        return;

//    isDragging_ = true;
//    isClimbing_ = false;
//    isFalling_ = false;
//    isStanding_ = false;
//    isWalking_ = false;
//    float x = pos.x() * PhysicsFactory::PixelsToMeters;
//    float y = pos.y() * PhysicsFactory::PixelsToMeters;

//    SetBodyPosition(x, y);
}

void OGBall::MouseUp(const QPoint &pos)
{
//    Q_UNUSED(pos)

//    if (_isSleeping)
//        return;

//    if (!isAttached_)
//        while (!jointBalls_.isEmpty())
//        {
//            Attache(jointBalls_.takeFirst());
//        }

//    isDragging_ = false;
//    GetBody()->body->SetAwake(true);
}

void OGBall::MouseMove(const QPoint &pos)
{
    float x = pos.x() * PhysicsFactory::PixelsToMeters;
    float y = pos.y() * PhysicsFactory::PixelsToMeters;

    SetBodyPosition(x, y);

    if (IsAttached())
        Detache();
    else if (GetMaxStrands() > 0)
        FindJointBalls();
}

void OGBall::SetMarked(bool status)
{
    isMarked_ = status;

    if (isMarked_)
        if (isWalking_ || isClimbing_)
            GetBody()->body->SetAwake(false);
}

bool OGBall::TestPoint(const QPoint &pos)
{
    float x = pos.x() * PhysicsFactory::PixelsToMeters;
    float y = pos.y() * PhysicsFactory::PixelsToMeters;

    return GetBody()->fixture->TestPoint(b2Vec2(x, y));
}

void OGBall::Algorithm2()
{
    if (_GetWorld()->nearestball() == 0)
        return;

    OGUserData* data;
    float dist1, dist2;

    b2JointEdge* joints = 0;
    joints = pTargetBall_->GetJoints();
    OGBall* ball = 0;
    b2Body* b1 = 0;
    b2Body* b2 = 0;
    auto phyPos = _GetWorld()->nearestball()->GetPhyPosition();
    b2Vec2 pos(phyPos.x(), phyPos.y());

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
        pClimbBehavior_->initNewTarget();
        SetOrigin(pTargetBall_);
        pOriginBall_=pTargetBall_;
        SetTarget(ball);
        pTargetBall_ = ball;
    }
}

inline float OGBall::DistanceSquared(OGBall* b1, OGBall* b2) const
{
    QVector2D v = b1->GetPhyPosition() - b2->GetPhyPosition();
    return v.lengthSquared();
}

inline float OGBall::DistanceSquared(OGBall* b)
{
    return DistanceSquared(b, this);
}

inline float OGBall::Distance(OGBall* b1, OGBall* b2) const
{
    QVector2D v = b1->GetPhyPosition() - b2->GetPhyPosition();

    return v.length();
}

inline float OGBall::Distance(OGBall* b)
{
    return Distance(b, this);
}

inline OGWorld* OGBall::_GetWorld()
{
    return OpenGOO::GetInstance()->GetWorld();
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
    auto ud = GetBody()->body->GetUserData();

    if (!ud)
       return nullptr;

    return static_cast<OGUserData*>(ud);
}

inline void OGBall::PerformWalk()
{
    pWalkBehavior_->Walk();
}

inline void OGBall::PerformClimb()
{
    pClimbBehavior_->Climb();
}

inline void OGBall::PerformFly()
{
    pFlyBehavior_->Fly();
}

inline void OGBall::SetWalkTarget(QPointF* pos)
{
    pWalkBehavior_->SetTarget(pos->x(), 0);
}

inline void OGBall::SetClimbTarget(QPointF* pos)
{
    pClimbBehavior_->SetTarget(*pos);
}

inline void OGBall::SetClimbOrigin(QPointF* pos)
{
    pClimbBehavior_->SetOrigin(*pos);
}

inline void OGBall::SetFlyTarget(QPointF* pos)
{
    pFlyBehavior_->SetTarget(*pos);
}

inline void OGBall::SetWalkSpeed(float speed)
{
    pWalkBehavior_->SetSpeed(speed);
}

inline void OGBall::SetClimbSpeed(float speed)
{
    pClimbBehavior_->SetSpeed(speed);
}

inline float OGBall::GetAngle() const
{
    return m_data->angle;
}

inline WOGBallShape* OGBall::GetShape() const
{
    return m_config->attribute.core.shape.get();
}

QString OGBall::GetStrandType() const
{
    const auto& strand = m_config->strand;
    if (!strand)
        return QString();
    else
        return strand->type;
}

inline bool OGBall::IsDetachable() const
{
    return m_config->attribute.player.detachable;
}

QString OGBall::GetId() const
{
    return m_data->id;
}

int OGBall::GetMaxStrands() const
{
    return m_config->attribute.core.strands;
}

OGIWalkBehavior* OGBall::GetWalkBehavior()
{
    if (!pWalkBehavior_)
        pWalkBehavior_ = new OGWalk(GetBody());

    return pWalkBehavior_;
}

OGIClimbBehavior* OGBall::GetClimbBehavior()
{
    if (!pClimbBehavior_)
        pClimbBehavior_ = new OGClimb(GetBody());

    return pClimbBehavior_;
}

OGIFlyBehavior* OGBall::GetFlyBehavior()
{
    if (!pFlyBehavior_)
        pFlyBehavior_ = new OGFly(GetBody());

    return pFlyBehavior_;
}

std::unique_ptr<BallSensor> OGBall::getSensor()
{
    return std::unique_ptr<BallSensor>(new BallSensor(this));
}

void OGBall::OnMouseDown()
{
    isDragging_ = true;
    GetBody()->SetActive(false);
}

void OGBall::OnMouseUp()
{
    isDragging_ = false;
    auto pos = QVector2D(OGWindowCamera::instance()->windowToLogical(og::MouseInput::GetPosition()));
    SetPosition(pos);
    GetBody()->SetPosition(pos * PhysicsFactory::PixelsToMeters);
    GetBody()->SetActive(true);
}

void OGBall::Added()
{
    GetBody()->SetActive(true);
}
