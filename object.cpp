#include "object.h"

Object::Object(QObject *parent) :
    QObject(parent)
{
}

b2Body* Object::getBody(){
    return body;
}

