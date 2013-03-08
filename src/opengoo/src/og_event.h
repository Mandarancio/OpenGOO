#ifndef OGEVENT_H
#define OGEVENT_H

#include <QList>
#include <QString>

class OGEvent
{    
public:
    enum Type
    {
          RESTART
        , RESUME
        , SHOW_OCD
        , BACKTO_ISLAND
        , BACKTO_MAINMENU
        , CREATE_MENU
        , LOAD_ISLAND
        , LOAD_LEVEL
    };

    OGEvent(Type type, QList<QString>* args=0);
    virtual ~OGEvent();

    Type type() const { return type_; }
    QList<QString>* args() { return args_; }

private:
    Type type_;
    QList<QString>* args_;

protected:
    void SetArgs(QList<QString>* args) { args_ = args; }
};

class OGIslandEvent : public OGEvent
{
public:
    OGIslandEvent(const QString &name);
};

class OGMenuEvent : public OGEvent
{
public:
    OGMenuEvent(const QString &name);
};

class OGLevelEvent : public OGEvent
{
public:
    OGLevelEvent(const QString &name);
};

#endif // OGEVENT_H
