#include "target.h"
#include "tools.h"

Target::Target(QPoint position, int height,b2World *world, QObject *parent) :
    QObject(parent)
{
    this->position=position;
    h=height;
    b2BodyDef def;
    def.type=b2_staticBody;
    def.position=toVec(position);
    b2PolygonShape shape;
    shape.SetAsBox(18,h/2,b2Vec2(0,-h/2),0);
    body=world->CreateBody(&def);
    body->CreateFixture(&shape,1.0);
    body->SetUserData(this);
    catched=false;
}

void Target::checkTower(QList<Goo *> ps){
    b2Vec2 d;
    bool check=false;
    for (int i=0;i<ps.length();i++){
        if (ps[i]->hasJoint()){
            d=toVec(position)-ps[i]->getVPosition();
            if (d.Length()<80 && !catched){
                emit this->towerCatch();
                check=true;
                catched=true;
            }
            else if (d.Length()<80) check=true;
        }
    }
    if (!check && catched) {
        catched=false;
        emit towerLost();
    }
}

void Target::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.drawRect(position.x()-18,position.y(),18*2,-h);
    p.drawRect(position.x()-22,position.y()+5,22*2,-15);
}

void Target::applyForce(Goo *goo){}
void Target::applyImpulse(Goo *goo){}
