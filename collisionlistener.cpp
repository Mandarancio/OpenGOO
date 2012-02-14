#include "collisionlistener.h"
#include "target.h"
#include "thorn.h"

#include "tools.h"

#include <QDebug>

CollisionListener::CollisionListener(QObject *parent) :
    QObject(parent)
{
    //Nothing to do here (for now)
}

void CollisionListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold){
    if (oldManifold->points==contact->GetManifold()->points) contact->SetEnabled(false); //To skip continuos contact
    Goo*a,*b; //for get the goo involved in the contact
    a=static_cast<Goo*>(contact->GetFixtureA()->GetUserData()); //Dynamic cast is for retrive (if any) the goo object of the first body involved
    b=static_cast<Goo*>(contact->GetFixtureB()->GetUserData()); //Dynamic cast is for retrive (if any) the goo object of the second body involved
    if (a&&b) { //If both bodies are goo
        contact->SetEnabled(false); //if the contact is between two Goo is ignored
        if (a->isFalling() && b->hasJoint()){  //if the first goo is falling down and the other is jointed with other one stop the falling
            a->setTarget(b);
        }
        else if (b->isFalling() && a->hasJoint()){ //if the second goo is falling down and the other is jointed with other one stop the falling
            b->setTarget(a);
        }
    }
    else { //cas if the contact is not between two goo
        Target * t; //target
        Thorn* th; //thorn
        if (!b){ //if the second object isn't a goo
            t=static_cast<Target*>(contact->GetFixtureB()->GetBody()->GetUserData()); //check if is the target
            th=static_cast<Thorn*>(contact->GetFixtureB()->GetBody()->GetUserData()); //check if is a thorn
            if (!t && !th&& a) { //if isn't any of the both and first body is a goo
                if (a){//advice the goo that he/it touch the ground
                    b2Vec2 p=contact->GetManifold()->localPoint;
                    p=a->getBody()->GetWorldPoint(p);
                    a->contactGround(toPoint(p));
                }
            }
            if (th) a->destroyThis(); //if is thorn destroy the goo!
        }
        else if (!a){ //if the first isn't a goo is the same of before!
            t=static_cast<Target*>(contact->GetFixtureA()->GetBody()->GetUserData());
            th=static_cast<Thorn*>(contact->GetFixtureA()->GetBody()->GetUserData());

            if (!t&&!th&&b) {
                if (b){
                    b2Vec2 p=oldManifold->localPoint;
                   // p.y=b->getVPosition().y-p.y;
                     // p.y=pa.y+p.y;
                    p= contact->GetFixtureA()->GetBody()->GetWorldPoint(p);
                    //p.x=b->getVPosition().x+p.x;
                    b->contactGround(toPoint(p));
                }
            }
            if (th) b->destroyThis();
        }
        contact->SetEnabled(true); //contact is enabled here
    }
}

