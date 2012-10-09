#include "backtracer.h"
#include <execinfo.h>
#ifdef __FreeBSD__
#	include <ucontext.h>
#endif

#include <QDebug>
#include <QString>

static void SignalHandler(int32_t sig, siginfo_t *info, void *scp);

void BackTracer(int32_t sig){
    struct sigaction sa;
    sa.sa_sigaction = SignalHandler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_NODEFER | SA_RESETHAND;
    sigaction(sig, &sa, NULL);
}

static void SignalHandler(int32_t sig, siginfo_t *info, void *scp){
    void * array[64];
    int32_t nSize = backtrace(array, 64);
    char ** symbols = backtrace_symbols(array, nSize);
    ucontext_t * ucx = static_cast<ucontext_t*>(scp);
    qWarning() << "signal " << sig << " si_code " << info->si_code << " si_addr " << hex
        << info->si_addr << dec << " ss_sp "
        << ucx->uc_stack.ss_sp;
    for (int32_t i = 2; i < nSize; i++)
        qWarning()<<symbols[i];

    free(symbols);
    signal(SIGABRT, SIG_DFL);
    exit(sig);
}

