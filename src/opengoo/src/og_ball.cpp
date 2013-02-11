#include "og_ball.h"
#include "physics.h"

extern OGWorld* _world;

OGBall::OGBall(WOGBallInstance *data, WOGBall *configuration)
    : m_data(data), m_configuration(configuration)
{
    attached = false;
    detaching = false;
    dragging = false;
    standing = false;
    walking = true;
    m_material = new WOGMaterial;
    m_material->bounce = 0.1f;
    m_material->friction = 15.0f;
    m_numberStrands = 0;

    if (configuration !=0 )
    {
        WOGBallShape* ballShape = GetShape();
        float32 x = data->x;
        float32 y = data->y;
        float32 angle = data->angle;
        float mass = GetMass();
        int vK = 100;
        int v = ballShape->variation*vK;
        m_K = 1/vK;

        if (v > vK) { v = vK; }

        OGPhysicsBody* obj = 0;

        if (ballShape->type == "circle")
        {
            m_type = OGBall::C_BALL;
            obj = m_CreateCircle(x, y, angle, mass, ballShape, v);
        }
        else if (ballShape->type == "rectangle")
        {
            m_type = OGBall::R_BALL;
            obj = m_CreateReactangle(x, y, angle, mass, ballShape, v);
        }

        if (obj !=0)
        {
            body = obj->body;
            fixture = obj->fixture;
            shape = obj->shape;
        }
    }
}

OGPhysicsBody* OGBall::m_CreateCircle(float32 x, float32 y, float32 angle
                                     , float mass, WOGBallShape* shape
                                     , int variation)
{
    WOGCircleBall* obj = static_cast<WOGCircleBall*>(shape);
    float32 radius = obj->radius;

    if (variation >= 1) { radius += radius*(qrand()%variation)*m_K; }

    return createCircle(x, y, radius, angle, m_material
                                      , true, mass, 0);

}

OGPhysicsBody* OGBall::m_CreateReactangle(float32 x, float32 y, float32 angle
                                         , float mass, WOGBallShape* shape
                                         , int variation)
{
    WOGRectangleBall* obj = static_cast<WOGRectangleBall*>(shape);
    float width = obj->width;
    float height = obj->height;

    if (variation >= 1)
    {
        width += width*(qrand()%variation)*m_K;
        height += height*(qrand()%variation)*m_K;
    }

    return createRectangle(x, y, width, height, angle, m_material
                                 , true, mass, 0);
}

void OGBall::SetBodyPosition(float32 x, float32 y)
{
    body->SetTransform(b2Vec2(x, y), body->GetAngle());
    body->SetAwake(false);
}

void OGBall::Detache()
{
    if (!IsDetachable()) { return; }

    b2JointEdge* joints = body->GetJointList();

    QList<OGStrand*> strands;

    while (joints)
    {
        strands << static_cast<OGStrand*>(joints->joint->GetUserData());
        joints = joints->next;
    }

    while (!strands.isEmpty())
    {
        _world->RemoveStrand(strands.takeFirst());
    }

    body->SetAwake(false);

    Event("detached");
}

void OGBall::Attache(OGBall* ball)
{
    _world->CreateStrand(this, ball);
    Event("attach");
}

bool OGBall::Select(const QPoint & pos)
{
    const qreal K = 10.0;

    if (m_type == OGBall::C_BALL)
    {
        qreal x1, y1, x2, y2, radius, length;
        x1 = GetX()*K;
        y1 = GetY()*K;
        x2 = pos.x();
        y2 = pos.y();
        radius = shape->GetRadius()*K;
        length = QLineF(x1, y1, x2, y2).length();

        if (length <= radius)
        {
            return true;
        }
    }

    return false;
}

void OGBall::Move(const QPointF & pos)
{
    if (dragging || attached || standing) { return; }

    if (walking)
    {
        Walk(pos);
    }
}

void OGBall::Walk(const QPointF & pos)
{
    if (testWalkable())
    {
        b2Vec2 vel = body->GetLinearVelocity();
        qreal dx = pos.x() - GetX();
        float32 speed = m_configuration->attribute.movement.walkspeed;

        if (dx >= 0) { vel.x = speed*80; }
        else { vel.x = -speed*80; }

        body->SetLinearVelocity(vel);
    }
}

bool OGBall::testWalkable()
{
    OGIBody* data;

    b2ContactEdge* edge = body->GetContactList();

    while (edge)
    {
        data = static_cast<OGIBody*>(edge->other->GetUserData());

        if (data)
        {
            if (data->walkable() && edge->contact->IsTouching())
            {
                return true;
            }
        }

        edge = edge->next;
    }

    return false;
}

void OGBall::Event(const QString & event)
{
    if (event == "pickup")
    {
        dragging = true;
        body->SetAwake(false);
    }
    else if(event == "attach")
    {

    }
    else if (event == "detached")
    {

    }
    else if (event == "detaching")
    {

    }
    else if (event == "marker")
    {
        standing = true;
        body->SetAwake(false);
    }
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

    if (attached)
    {
        pen.setColor(Qt::blue);
        painter->setPen(pen);
    }

    painter->drawEllipse(QPointF(x, y), radius, radius);
    painter->restore();
}
