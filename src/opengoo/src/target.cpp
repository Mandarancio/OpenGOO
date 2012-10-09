#include "target.h"

Target::Target(QPoint position, int height,b2World *world, QObject *parent) :
    QObject(parent)
{
    this->position=position;
    h=height;
    b2BodyDef def;
    def.type=b2_staticBody;
    def.position=toVec(position);
    b2PolygonShape shape;
    shape.SetAsBox(1.8,h/2,b2Vec2(0,-h/2),0);
    body=world->CreateBody(&def);
    body->CreateFixture(&shape,1.0);
    body->SetUserData(this);
    catched=false;
}

//Function to know if the goo tower is in proximity of the target
//the function check all the jointed goo, if someone is at least at 80px from the target, the tower is catched!
void Target::checkTower(QList<Goo *> ps){
    b2Vec2 d;
    bool check=false;
    for (int i=0;i<ps.length();i++){
        if (ps[i]->hasJoint() && ps[i]->canHaveGuest()){
            d=toVec(position)-ps[i]->getVPosition();
            if (d.Length()<80/10 && !catched){
                emit this->towerCatch();
                check=true;
                catched=true;
            }
            else if (d.Length()<80/10) check=true;
        }
    }
    if (!check && catched) {
        catched=false;
        emit towerLost();
    }
}

//function to apply forces and impulses at all the goos
void Target::applyForces(QList<Goo *> ps){
    if (catched){
        for (int i=0;i<ps.length();i++){
            if (ps[i]->hasJoint()){
                applyForce(ps[i]);
            }
            else applyImpulse(ps[i]);
        }
    }
}

//Function to paint the target
//In future should be better!
void Target::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.drawRect(position.x()-18,position.y(),18*2,-h);
    p.drawRect(position.x()-22,position.y()+5,22*2,-15);
}

//Function to apply force at the goo tower.
void Target::applyForce(Goo *goo){
    if (catched && goo->hasJoint()){
        b2Vec2 d;
        d=toVec(position)-goo->getVPosition();
        if (d.Length()>20) return;
        d.x/=d.Length();
        d.y/=d.Length();
        d*=60/d.Length()*goo->getBody()->GetMass();
        goo->getBody()->ApplyForceToCenter(d);
    }
}

//Apply impulse to jump at free goo near at the target
void Target::applyImpulse(Goo *goo){
    //check if the goo is not jointed and not dragged and if the target has catched the goo tower.
    if (!goo->hasJoint() && !goo->isDragging() && !goo->isSleeping() && catched){
        b2Vec2 d=toVec(position)-goo->getVPosition();
        //if the goo is at least at 15px from the target is catched
        if (d.Length()<=0.5+goo->getRadius()/10.0){
            emit gooCatched(goo);
        }
        //else if at least at 80px jump!
        else if (d.Length()<8){
            goo->jumpTo(position);
        }

    }
}
