#ifndef OG_DISPATCHER_H
#define OG_DISPATCHER_H

#include "og_eventhandler.h"

template<typename Dummy_T = void>
class OGDispatcher
{
public:
    static bool EventDispatch(OGEvent::Type type);
};

#include "eventhandlers.h"

#endif // OG_DISPATCHER_H
