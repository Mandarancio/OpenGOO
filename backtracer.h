#ifndef BACKTRACER_H
#define BACKTRACER_H

#include <stdint.h>
#include <csignal>
static struct sigaction oldh[32];
static void SignalHandler(int32_t sig,siginfo_t *info,void *scp);

class BackTracer
{
public:
    BackTracer(int32_t sig);

};

#endif // BACKTRACER_H
