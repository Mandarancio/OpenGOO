#include "collisionlistener.h"
#include "target.h"
#include "thorn.h"

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
        Thorn* th;
        if (!b){
            t=static_cast<Target*>(contact->GetFixtureB()->GetBody()->GetUserData());
            th=static_cast<Thorn*>(contact->GetFixtureB()->GetBody()->GetUserData());
            if (!t && !th&& a) {
                if (a&&a->isFalling())a->contactGround();
            }
            if (th) a->destroyThis();
        }
        else if (!a){
            t=static_cast<Target*>(contact->GetFixtureA()->GetBody()->GetUserData());
            th=static_cast<Thorn*>(contact->GetFixtureA()->GetBody()->GetUserData());

            if (!t&&!th&&b) {
                if (b&&b->isFalling())b->contactGround();
            }
            if (th) b->destroyThis();
        }
        contact->SetEnabled(true);
    }
}

