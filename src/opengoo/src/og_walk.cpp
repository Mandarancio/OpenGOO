#include "og_walk.h"
#include <OGPhysicsBody>

using namespace og;

struct OGWalkImpl
{
    OGPhysicsBody* pBody;
};

OGWalk::OGWalk(OGPhysicsBody* body)
{
    pImpl_ = new OGWalkImpl;
    pImpl_->pBody = body;
}

OGWalk::~OGWalk()
{
    delete pImpl_;
}

void OGWalk::_Walk()
{
    QVector2D vel = pImpl_->pBody->GetVelocity();
    float dx = position.x() - pImpl_->pBody->GetX();

    if (dx >= 0) { vel.setX(speed * 80); }
    else { vel.setX(-speed * 80); }

    pImpl_->pBody->SetVelocity(vel);
}
