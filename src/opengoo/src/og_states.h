#ifndef OG_STATES_H
#define OG_STATES_H

enum OGState
{
    RESTART
    , BACKTO
    , RESUME
    , SHOW_OCD
    , CREATE_MENU
    , REMOVE_MENU
    , RETRY
    , REMOVE_RETRY_MENU
};

enum OGEvent
{
    MOUSE_MOVE
    , MOUSE_DOWN
    , LEAVE
};

extern QList<OGState> _listStates;

#endif // OG_STATES_H
