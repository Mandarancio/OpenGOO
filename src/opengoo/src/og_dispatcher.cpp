#include "og_dispatcher.h"
#include "og_eventhandler.h"

bool OGDispatcher<>::EventDispatch(OGEvent::Type type)
{
    switch (type)
    {
        case OGEvent::EXIT:
        {
            OGEventHandler<OGEvent::EXIT>::Event();
            return true;
        }
        case OGEvent::CONTINUE:
        {
            OGEventHandler<OGEvent::CONTINUE>::Event();
            return true;
        }
        default :
        {
            return false;
        }
    }
}
