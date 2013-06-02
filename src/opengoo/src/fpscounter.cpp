#include "fpscounter.h"

//struct FPSCounter::Impl
//{
//    int fps;
//    int curFPS;
//    int time;
//};

FPSCounter::FPSCounter() : _pImpl(new Impl)
{    
    reset();
}

// Dtor prevents error C2027
FPSCounter::~FPSCounter() {}

void FPSCounter::reset()
{
    _pImpl->time = 0;
    _pImpl->curFPS = 0;
    _pImpl->fps = 0;
}

void FPSCounter::update(int dt)
{
    _pImpl->curFPS++;
    _pImpl->time += dt;

    if (_pImpl->time >= 1000)
    {
        _pImpl->fps = _pImpl->curFPS;
        _pImpl->curFPS = 0;
        _pImpl->time = 0;

        emit setFPS(_pImpl->fps);
    }
}

int FPSCounter::getFPS() { return _pImpl->fps; }
