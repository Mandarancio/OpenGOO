#include "og_event.h"

OGEvent::OGEvent(Type type, QList<QString>* args) : type_(type), args_(args)
{    
}

OGEvent::~OGEvent()
{
    if (args_ != 0) delete args_;
}


OGIslandEvent::OGIslandEvent(const QString& name)
    : OGEvent(OGEvent::LOAD_ISLAND)
{
    QList<QString>* args = new QList<QString>;
    args->push_back(QString(name));
    SetArgs(args);
}

OGMenuEvent::OGMenuEvent(const QString& name)
    : OGEvent(OGEvent::CREATE_MENU)
{
    QList<QString>* args = new QList<QString>;
    args->push_back(QString(name));
    SetArgs(args);
}

OGLevelEvent::OGLevelEvent(const QString& name)
    : OGEvent(OGEvent::LOAD_LEVEL)
{
    QList<QString>* args = new QList<QString>;
    args->push_back(QString(name));
    SetArgs(args);
}
