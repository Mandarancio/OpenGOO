#include "collisionlistener.h"
#include "target.h"

CollisionListener::CollisionListener(QObject *parent) :
    QObject(parent)
{
}

void CollisionListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold){
    Goo*a,*b;
    a=static_cast<Goo*>(contact->GetFixtureA()->GetUserData());
    b=static_cast<Goo*>(contact->GetFixtureB()->GetUserData());
    if (a&&b) contact->SetEnabled(false);
    else {
        Target * t;
        if (!b){
            t=static_cast<Target*>(contact->GetFixtureB()->GetBody()->GetUserData());
            if (!t && a) {
                if (a&&a->isFalling())a->contactGround();
            }
        }
        else if (!a){
            t=static_cast<Target*>(contact->GetFixtureA()->GetBody()->GetUserData());
            if (!t&&b) {
                if (b&&b->isFalling())b->contactGround();

            }
        }
        contact->SetEnabled(true);
    }
}

