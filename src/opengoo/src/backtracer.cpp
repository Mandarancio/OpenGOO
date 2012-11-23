#include "backtracer.h"
#include <execinfo.h>
#ifdef __FreeBSD__
#	include <ucontext.h>
#endif

#include <QDebug>
#include <logger.h>
#include <QString>
#include <QProcess>

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

    QString backtraceText;
    QTextStream backtraceStream(&backtraceText);

    backtraceStream << "signal " << sig << " si_code " << info->si_code << " si_addr " << hex
        << info->si_addr << dec << " ss_sp "
        << ucx->uc_stack.ss_sp;

    for (int32_t i = 2; i < nSize; i++)
        backtraceStream<<symbols[i];

    free(symbols);
    signal(SIGABRT, SIG_DFL);

    //At crash the report is generated and OpenGooDst is launched.

    CrashXmlModule *reportModule = new CrashXmlModule(backtraceText);
    QUuid reportQUuid = reportModule->getUuid();

    logWarn(QString("back Report quuid %1").arg(reportQUuid.toString()));

    QProcess *myProcess = new QProcess;

    QString program = "./OpenGooDst";
    QStringList arguments;
    arguments << "-w" << "-Uuid=" + reportQUuid.toString();

    myProcess->start(program, arguments);

    myProcess->waitForFinished();

    delete reportModule;

    exit(sig);
}

