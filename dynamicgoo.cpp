#include "dynamicgoo.h"
#include "tools.h"

#include <QDebug>

#include <qmath.h>

DynamicGoo::DynamicGoo(b2World *world, QPoint p, int radius,  QObject *parent):
    Goo(radius,parent)
{
    this->world=world; //get a copy of the world
    sticked=false; //start not sticked at ground
    color=Qt::white; //the recognize color of normal dynamic goo is white;
    secondaryColor=color;
    b2BodyDef def; //body definition
    def.awake=true; //is active
    def.type=b2_dynamicBody; //is dynamci (react at force impulse and collision)
    def.position=toVec(p); //set the position
    body=world->CreateBody(&def); //create the physical body
    b2CircleShape shape; //define the shape
    shape.m_p.SetZero(); //position of the shape
    shape.m_radius=radius; //radius

    b2FixtureDef fixDef; //Definition of the phisical parameters
    fixDef.restitution=0.3; //collision restitution
    fixDef.density=0.0; //density
    fixDef.friction=0.8; //friction
    fixDef.shape=&shape; //assign the shape

    fixDef.userData=this; //assign a copy of  the object at the body so during the contact is possible to know the info of the goo
    body->CreateFixture(&fixDef); //create the fixture
    body->SetLinearDamping(0.1);//Not sure about this parameter
    moovable=true; //flags
    dragable=true;
    maxJoints=7; //parameters
    speed=50;
    type=DYNAMIC;

}

void DynamicGoo::catched(){
    speed*=2;
}

void DynamicGoo::lost(){
    speed/=2;
}

void DynamicGoo::moveToTarget(){
    //Stop to follow if the goo is dragged for the user
    if (isDragging()) {
        stopFollow();
        return;
    }
    if (onGround && groundPoint!=getPPosition()) {
        onGround=false;
    }
    if (isFalling()) return;
    if (hasJoint()) return;
    if (!hasJoint() && !isDragging() && isOnGround() && target==NULL ){

        emit this->nextTargetPlease(NULL);
        qWarning()<<"NEXT TARGET PLEASE!!";
    }
    if (following && !falling){
        if (prevTarget){
            if (!target->isLinked(prevTarget)){
                drop();
                return;
            }
            QPoint meTarget=target->getPPosition()-getPPosition();
            if (toVec(meTarget).Length()>150) {
                stopFollow();
                fallDown();
                return;
            }

            //Compute mx and my for the position
            float mx,my;
            mx=(target->getVPosition().x-prevTarget->getVPosition().x);
            my=(target->getVPosition().y-prevTarget->getVPosition().y);
            float d=qSqrt(mx*mx+my*my);
            mx/=d;
            my/=d;
            float tx,ty;
            //Compute tehorical x and y
            tx=(getVPosition().x-prevTarget->getVPosition().x)/mx;
            ty=(getVPosition().y-prevTarget->getVPosition().y)/my;
            float yt=my*tx+prevTarget->getVPosition().y;
            float xt=mx*ty+prevTarget->getVPosition().x;

            //if my y position is different at least of 12 falldown and return
            if (qAbs(getVPosition().y-yt)>=getRadius() && qAbs(getVPosition().x-xt)>=getRadius()){
                stopFollow();
                fallDown();
                return;
            }
        }
        //this is a work around for the very ugly bug of the "FLYING GOOS"
        //description of the bug if no prevTarget is setted but target is and the goo tower start to fall down the goo start to fly
        //for reach his target!
        //check if prevtarget is not setted and target is
        if (!prevTarget && target && !target->isOnGround()){
            //compute distance between target and me
            float d=(target->getVPosition()-getVPosition()).Length();
            //if that distance is more than 25 falldown and return
            //PS: remember that 30 is a single point contact between two goos
            if (d>radius*5/3) {
                stopFollow();
                fallDown();
                return;
            }
        }
        b2Vec2 dP;
        if (target!=NULL /*&& target->getBody()!=NULL*/)
            dP=target->getVPosition()-this->getVPosition();
        else {
            stopFollow();
            fallDown();
            return;
        }
        if (dP.Length()<=radius/2){
            emit this->nextTargetPlease(target);
            body->SetLinearVelocity(b2Vec2(0,0));
            return;
        }
        if (!prevTarget && onGround){
            dP.x=(dP.x>0 ? speed*5 : -speed*5);
            dP.y=body->GetWorld()->GetGravity().y;
            body->ApplyForceToCenter(dP);
        }
        else {
            dP.x=dP.x*speed/dP.Length();
            dP.y=dP.y*speed/dP.Length() ;
            body->SetLinearVelocity(dP);
        }
    }
}


void DynamicGoo::paint(QPainter &p){
    //Check rutine
    if (!isSleeping()){

        moveToTarget();
    }
    else
        emit this->checkForNeighbors(getPPosition());
    //Real paint stuff
    //check if is dragged
    if (isDragging()){
        p.setBrush(Qt::transparent);
        p.setPen(QPen(color,3));
        p.drawEllipse(toPoint(body->GetPosition()), getRadius()+10,getRadius()+10);
    }
    //check if is selected and draggable
    else if (selected && isDragable()){
        if (!hasJoint())
            p.setPen(QPen(color,3,Qt::DashLine));
        else
            p.setPen(QPen(secondaryColor,3,Qt::DotLine));
        p.setBrush(Qt::transparent);
        p.drawEllipse(toPoint(body->GetPosition()), getRadius()+10,getRadius()+10);

    }
    //paint goo
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    p.drawEllipse(getPPosition(),getRadius(),getRadius());

}

void DynamicGoo::paintDebug(QPainter &p){
    //set the pen green if is jointed or white if is free and not sleeping
    p.setPen( Qt::white);
    //if has joint draw the number of joint of it inside him.
    if (hasJoint()){
        target=NULL;
        prevTarget=NULL;
        p.drawText(QPoint(getPPosition().x()-5,getPPosition().y()+5),QString::number(this->nJoints()));
    }
    //if not and is free and is mooving to reach a target draw the direction
    else if (target!=NULL){
        //save the position
        p.save();
        //translate the painter at the center of the goo
        p.translate(getPPosition());
        //rotate the painter of the angle between the goo and his target
	p.rotate(atan2(static_cast<float>(target->getPPosition().y()-getPPosition().y()),static_cast<float>(target->getPPosition().x()-getPPosition().x()))*180.0/3.141628);
        //draw a line in this direction of 40 px
        p.drawLine(0,0,40,0);

        //translate the painter at the end of the line
        p.translate(40,0);
        //rotatate of 45° degree
        p.rotate(45);
        //draw a line of 5 (this is one side of the arrow)
        p.drawLine(0,0,-5,0);
        //rotate to -45° degree
        p.rotate(270);
        //draw the other side of the arrow
        p.drawLine(0,0,-5,0);
        //restore the previous position

        p.restore();
    }
}

//rutine for wake up goo!
void DynamicGoo::neighborsFound(){
    //set flag false
    sleeping=false;
    //call rutine for move to next goo
    moveToTarget();
}

void DynamicGoo::unstick(){
    sticked=false;
}

void DynamicGoo::contactGround(){
    //Away to trapass body
    if (isDragging()) {
        //Change flag and reset normal status
        dragging=false;
        fallDown();
        //Emit a signal for the level class
        emit stopDragging();
        return;
    }
    else {
        onGround=true;
        groundPoint=this->getPPosition();
        //if has joint return;
        if (hasJoint()){

            return;
        }
        if (sleeping) {
            body->SetGravityScale(1.0);
            return;
        }
        if(!hasJoint() && target==NULL){
            emit this->nextTargetPlease(NULL);
        }
    }
}

void DynamicGoo::contactGround(QPoint p){
    //Away to trapass body
    if (isDragging()) {
        //Change flag and reset normal status
        dragging=false;
        fallDown();
        //Emit a signal for the level class
        emit stopDragging();
        return;
    }
    else {
        onGround=true;
        groundPoint=this->getPPosition();

        //if has joint and is not sticked on ground
        if (hasJoint()){
            if (!sticked){
                onGround=true;
                groundPoint=this->getPPosition();
                emit this->createSticky(p);
                sticked=true; //flag to true
            }
            return;
        }
        if (sleeping) {
            body->SetGravityScale(1.0);
            return;
        }
        if(!hasJoint() && target==NULL){
            emit this->nextTargetPlease(NULL);
        }
    }
}

bool DynamicGoo::isDragable(){
    if (!hasJoint() && !isSleeping()) return true;
    else return false;
}
