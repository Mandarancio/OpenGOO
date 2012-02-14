#include "collisionlistener.h"
#include "target.h"
#include "thorn.h"

#include "tools.h"

#include <qmath.h>
#include <QDebug>

CollisionListener::CollisionListener(QObject *parent) :
    QObject(parent)
{
    //Nothing to do here (for now)
}

void CollisionListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold){
    //if (oldManifold->points==contact->GetManifold()->points) contact->SetEnabled(false); //To skip continuos contact

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
                    if ((p-a->getVPosition()).Length()>50) {
                        p=a->getVPosition();
                        p.y+=20;
                    }
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
                    //RETRIVE COLLISION POINT
                    b2Vec2 p=oldManifold->localPoint;
                    //CHANGE CORDINATE SYSTEM
                    p= contact->GetFixtureA()->GetBody()->GetWorldPoint(p);
                    //p.x=b->getVPosition().x+p.x;
                    //FOR UNKNOW REAZON SOMETIMES THE COLLISION POINT IS COMPLITLY WRONG
                    //SO I MADE THIS WORK AROUND TO FIX IT
                    //WORKAROUND
                    //CHECK IF THE COLLISION POINT IS TOO DISTANT FROM MY BODY FOR BE A CORRECT POINT
                    if ((p-b->getVPosition()).Length()>50) {
                        //SOSTITUITION OF THE COLLISION POINT WITH MY BODY POINT
                        p=b->getVPosition();
                        //RETRIVE THE NORMAL OF THE COLLISION
                        b2Vec2 n=oldManifold->localNormal;
                        //NORMALY THE NORMAL IS CORRECT
                        //BUT I EXPERIMENTED SOME ERROR ALSO WITH THE NORMAL
                        //CHECK IF Y COMPONTENT OF THE NORMAL IS BIGGER OF THE X
                        if (fabs(n.y)>=fabs(n.x)){
                            //CHECK IF THE Y COMPONENT IS BIGGER THAN 0 SO ADD -20 TO P.Y
                            if (n.y>0) p.y-=20;
                            //ELSE ADD +20 TO P.Y
                            else p.y+=20;
                        }
                        //ELSE
                        else{
                            //CHECK IF X COMPONENT IS BIGGER THE 0 SO ADD -20 TO P.X
                            if (n.x>0) p.x-=20;
                            //ELSE ADD +20 TO P.X
                            else p.x+=20;
                        }
                    }
                    //END WORKAROUND
                    //CALL CONTACTGROUND WITH THE CALCULATED P
                    b->contactGround(toPoint(p));
                }
            }
            if (th) b->destroyThis();
        }
        contact->SetEnabled(true); //contact is enabled here
    }
}

