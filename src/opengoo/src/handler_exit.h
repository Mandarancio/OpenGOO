#ifndef HANDLER_EXIT_H
#define HANDLER_EXIT_H

#include "GameEngine/og_gameengine.h"

template<>
struct OGEventHandler<OGEvent::EXIT>
{
    static void Event()
    {
        OGGameEngine::getEngine()->getWindow()->close();
    }
};

#endif // HANDLER_EXIT_H
