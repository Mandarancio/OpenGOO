#include "og_climb.h"
#include <OGPhysicsBody>

using namespace og;

struct OGClimbImpl
{
    OGPhysicsBody* pBody;
};

OGClimb::OGClimb(OGPhysicsBody* body)
{
    pImpl_ = new OGClimbImpl;
    pImpl_->pBody = body;
}

OGClimb::~OGClimb()
{
    delete pImpl_;
}

void OGClimb::_Climb()
{
    QVector2D dv = position - pImpl_->pBody->GetPosition();
    dv.normalize();
    QVector2D vs = speed * 3 * dv;
    pImpl_->pBody->SetVelocity(vs);
}


void OGClimb::onTargetChanged()
{
    if (!isNewTarget)
    {
        //get ball distance from origin
        float dist = (pImpl_->pBody->GetPosition() - (origin + originCorrection)).length();
        //calculate and normalize the new path
        QVector2D nv = position - (origin + originCorrection);
        nv.normalize();
        //multiply the new path with the distance
        nv*=dist;
        //place ball to the path
        pImpl_->pBody->body->SetTransform(b2Vec2((origin+originCorrection + nv).x(),(origin+originCorrection + nv).y()),pImpl_->pBody->body->GetAngle());
    } else {
        isNewTarget=false;
    }
}

void OGClimb::onOriginChanged()
{
    if (isNewOrigin)
    {
        //calculate origin correction
        originCorrection = pImpl_->pBody->GetPosition() - origin;
        isNewOrigin = false;
    }
}

void OGClimb::initNewTarget()
{
    isNewTarget=true;
    isNewOrigin=true;

}
