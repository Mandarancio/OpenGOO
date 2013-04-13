#include "wog_pipe.h"

WOGPipe::WOGPipe() : id("0"), depth(0), vertex(0) {}
WOGPipe::~WOGPipe()
{
    if (vertex)
    {
       vertex->clear();
       delete vertex;
    }
}
