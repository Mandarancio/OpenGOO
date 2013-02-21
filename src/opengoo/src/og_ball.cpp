#include "og_ball.h"
#include "physics.h"
#include "logger.h"

extern OGWorld* _world;
extern OGBall* _nearestBall;

OGBall::OGBall(WOGBallInstance *data, WOGBall *configuration)
    : data_(data), config_(configuration)
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
    isDraggable_ = config_->attribute.player.draggable;

    material_ = new WOGMaterial;
    material_->bounce = 0.1f;
    material_->friction = 15.0f;
    numberStrands_ = 0;
    id_ = -1;
    isInit_ = false;
    targetBall_ = 0;
    target_ = new QPointF;
    curPos_ = new QPointF;

    WOGBallShape* ballShape = GetShape();

    // Get position, angle and mass of ball
    float32 x = data_->x;
    float32 y = data_->y;
    float32 angle = data_->angle;
    float mass = config_->attribute.core.mass;
    int v = ballShape->variation*100; // convert the variation to a percentage

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

    if (obj !=0)
    {
        body = obj->body;
        fixture = obj->fixture;
        shape = obj->shape;
    }

    towerMass_ = config_->attribute.core.towermass*K;
}

OGPhysicsBody* OGBall::CreateCircle(float x, float y, float angle
                                     , float mass, WOGBallShape* shape
                                     , int variation)
{
    WOGCircleBall* obj = static_cast<WOGCircleBall*>(shape);
    float32 radius = obj->radius;

    if (variation >= 1) { radius += radius*(qrand()%variation)*0.01f; }

    OGUserData* data = new OGUserData;
    data->type = OGUserData::BALL;
    data->data = this;

    return createCircle(x, y, radius, angle, material_, true, mass, data);
}

OGPhysicsBody* OGBall::CreateReactangle(float x, float y, float angle
                                         , float mass, WOGBallShape* shape
                                         , int variation)
{
    WOGRectangleBall* obj = static_cast<WOGRectangleBall*>(shape);
    float width = obj->width;
    float height = obj->height;

    if (variation >= 1)
    {
        width += width*(qrand()%variation)*0.01f;
        height += height*(qrand()%variation)*0.01f;
    }

    return createRectangle(x, y, width, height, angle, material_, true, mass
                           , 0);
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

    while (!strands.isEmpty()) { _world->RemoveStrand(strands.takeFirst()); }

    body->SetAwake(false);
}

void OGBall::Attache(OGBall* ball) { _world->CreateStrand(this, ball); }

void OGBall::Attache()
{
    while (!jointBalls_.isEmpty()) { Attache(jointBalls_.takeFirst()); }
}

void OGBall::SetCurrentPosition(const b2Vec2 & pos)
{
    curPos_->setX(pos.x);
    curPos_->setY(pos.y);   
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
    const float K = 0.1f;
    float x, y;    

    if (isClimbing_)
    {
        float x1 = GetCurrentPosition()->x();
        float y1 = GetCurrentPosition()->y();
        float x2 = _world->leveldata()->levelexit->pos.x()*K;
        float y2 = _world->leveldata()->levelexit->pos.y()*K;
        float r1 = _world->leveldata()->levelexit->radius*K;
        float r2 = shape->GetRadius();
        float l1 = (r1 + r2)*(r1 + r2);
        float l2 = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);

        if (l2 <= l1 )
        {
            x = x2;
            y = y2;
        }
        else
        {
            if (IsCanClimb())
            {
                Algorithm2();
            }

            x = GetTarget()->x();
            y = GetTarget()->y();
        }

        if (targetBall_->IsAttached()) { Climbing(x, y); }
        else { isClimbing_ = false; }
    }
    else if (isWalking_)
    {
        FindTarget();
        Walk(GetTarget()->x());
    }
}

inline void OGBall::Climbing(float x, float y)
{
    b2Vec2 vel = body->GetLinearVelocity();
    float32 speed = config_->attribute.movement.climbspeed;
    QLineF line(*GetCurrentPosition(), QPointF(x, y));
    line.setLength(speed*3);
    vel.x = line.dx();
    vel.y = line.dy();
    body->SetLinearVelocity(vel);
}

inline void OGBall::Walk(float x)
{
    b2Vec2 vel = body->GetLinearVelocity();
    qreal dx = x - GetX();
    float32 speed = config_->attribute.movement.walkspeed;

    if (dx >= 0) { vel.x = speed*80; }
    else { vel.x = -speed*80; }

    body->SetLinearVelocity(vel);
}

inline bool OGBall::IsCanClimb()
{
    b2Vec2 pos(GetTarget()->x(), GetTarget()->y());

    if (fixture->TestPoint(pos) && targetBall_->IsAttached())
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

    qreal x, y, radius, angle;
    QLineF line;

    const qreal K = 10.0;
    const qreal DEGREE = 57.2957795;

    x = GetX()*K;
    y = GetY()*K*(-1.0);
    radius = shape->GetRadius()*K;
    angle = body->GetAngle()*DEGREE;
    line.setP1(QPointF(x, y));
    line.setP2(QPointF(x+radius, y));
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
    else if(isFalling_)
    {
        pen.setColor(Qt::green);
        painter->setPen(pen);
    }
    else if(isStanding_)
    {
        pen.setColor(Qt::yellow);
        painter->setPen(pen);
    }

    if (isMarked_)
    {
        pen.setColor(Qt::magenta);
        painter->setPen(pen);
    }

    painter->drawEllipse(QPointF(x, y), radius, radius);
    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->setFont(QFont("Times", 12, QFont::Bold));
    painter->drawText(QPointF(x, y), QString::number(id()));

    int x1, y1, x2, y2;

    Q_FOREACH (OGBall* ball, jointBalls_)
    {
        x1 = GetX()*K;
        y1 = GetY()*K*(-1.0);
        x2 = ball->GetX()*K;
        y2 = ball->GetY()*K*(-1.0);
        painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }

    painter->restore();
}

inline void OGBall::SetTarget(OGBall* target)
{
    target_->setX(target->GetX());
    target_->setY(target->GetY());
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
        isDraggable_ = config_->attribute.player.draggable;
    }
}

inline void OGBall::FindJointBalls()
{   
    float dist;
    float minlen = config_->stand->minlen;
    float maxlen1 = config_->stand->maxlen1;

    jointBalls_.clear();

    Q_FOREACH (OGBall* ball, *_world->balls())
    {
        if (ball->IsAttached())
        {
            dist = b2Distance(GetBodyPosition(), ball->GetBodyPosition())*10;

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
    float dist1, dist2 = 0;
    float isInit = false;

    b2Vec2 pos(GetBodyPosition());

    Q_FOREACH (OGBall* ball, *_world->balls())
    {
        if (ball->IsStanding())
        {
            if (!isInit)
            {
                nearestBall = ball;
                dist1 = b2Distance(pos, ball->GetBodyPosition());
                isInit = true;
            }
            else
            {
                dist2 = b2Distance(pos, ball->GetBodyPosition());

                if (dist2 < dist1)
                {
                    dist1 = dist2;
                    nearestBall = ball;
                }
            }
        }
    }

    SetTarget(nearestBall);
    targetBall_ = nearestBall;
}

// Pickup ball

void OGBall::MouseDown(const QPoint & pos)
{       
    const float K = 0.1f;

    isDragging_ = true;
    isClimbing_ = false;
    isFalling_ = false;
    isStanding_ = false;
    isWalking_ = false;
    float x = pos.x()*K;
    float y = pos.y()*K;

    SetBodyPosition(x, y);
}

void OGBall::MouseUp(const QPoint & pos)
{
    Q_UNUSED(pos)

    if (!isAttached_)
    {
        while(!jointBalls_.isEmpty()) { Attache(jointBalls_.takeFirst()); }
    }

    isDragging_ = false;
    body->SetAwake(true);
}

void OGBall::MouseMove(const QPoint & pos)
{
    const float K = 0.1f;
    float x = pos.x()*K;
    float y = pos.y()*K;

    SetBodyPosition(x, y);

    if (IsAttached()) { Detache(); }
    else if (GetMaxStrands() > 0)
    { FindJointBalls(); }
}

void OGBall::Select()
{
    if (isMarked_)
    {
        if (IsClimbing() || IsWalking()) { body->SetAwake(false); }
    }
}

bool OGBall::TestPoint(const QPoint & pos)
{
    const float K = 0.1f;

    float x = pos.x()*K;
    float y = pos.y()*K;

    return fixture->TestPoint(b2Vec2(x, y));
}

void OGBall::Algorithm2()
{
    b2JointEdge* joints = 0;
    joints = targetBall_->GetJoints();

    OGUserData* data;  
    float dist1, dist2;
    bool init = false;
    OGBall* b1 = 0;
    b2Body* bb1 = 0;
    b2Body* bb2 = 0;
    b2Vec2 pos(_world->nearestball()->GetBodyPosition());

    while (joints)
    {
        if (!init)
        {
            bb1 = joints->other;
            dist1 = b2Distance(bb1->GetPosition(), pos);
            init = true;
        }
        else
        {
            bb2 = joints->other;
            dist2 = b2Distance(bb2->GetPosition(), pos);

            if (dist2 < dist1)
            {
                dist1 = dist2;
                bb1 = bb2;
            }
        }

        joints = joints->next;
    }

    if (bb1 != 0)
    {
        data = static_cast<OGUserData*>(bb1->GetUserData());
        b1 = static_cast<OGBall*>(data->data);
        SetTarget(b1);
        targetBall_ = b1;
    }
}
