#include "og_fly.h"

using namespace og;

struct OGFlyImpl
{
    PhysicsBody* pBody;
};

OGFly::OGFly(PhysicsBody* body)
{
    pImpl_ = new OGFlyImpl;
    pImpl_->pBody = body;
}

OGFly::~OGFly()
{
    delete pImpl_;
}
