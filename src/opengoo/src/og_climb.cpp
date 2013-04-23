#include "og_climb.h"
#include "OGPhysicsBody"

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
