/*
* file   : backtracer_win32.cpp created 2012.6.25
* class  : backtracer_win32
* author : Slyshyk Oleksiy ( alexSlyshyk@gmail.com )
*/

#include "backtracer_win32.h"
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <DbgHelp.h>

#define EX_CASE(code) \
     case code:\
      return _T(#code);

const DWORD MS_VC_EXCEPTION=0x406D1388;

LPTSTR GetExceptionName(DWORD code)
{
    switch(code)
    {
        EX_CASE(EXCEPTION_ACCESS_VIOLATION);
        EX_CASE(EXCEPTION_DATATYPE_MISALIGNMENT);
        EX_CASE(EXCEPTION_BREAKPOINT);
        EX_CASE(EXCEPTION_SINGLE_STEP);
        EX_CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
        EX_CASE(EXCEPTION_FLT_DENORMAL_OPERAND);
        EX_CASE(EXCEPTION_FLT_DIVIDE_BY_ZERO);
        EX_CASE(EXCEPTION_FLT_INEXACT_RESULT);
        EX_CASE(EXCEPTION_FLT_INVALID_OPERATION);
        EX_CASE(EXCEPTION_FLT_OVERFLOW);
        EX_CASE(EXCEPTION_FLT_STACK_CHECK);
        EX_CASE(EXCEPTION_FLT_UNDERFLOW);
        EX_CASE(EXCEPTION_INT_DIVIDE_BY_ZERO);
        EX_CASE(EXCEPTION_INT_OVERFLOW);
        EX_CASE(EXCEPTION_PRIV_INSTRUCTION);
        EX_CASE(EXCEPTION_IN_PAGE_ERROR);
        EX_CASE(EXCEPTION_ILLEGAL_INSTRUCTION);
        EX_CASE(EXCEPTION_NONCONTINUABLE_EXCEPTION);
        EX_CASE(EXCEPTION_STACK_OVERFLOW);
        EX_CASE(EXCEPTION_INVALID_DISPOSITION);
        EX_CASE(EXCEPTION_GUARD_PAGE);
        EX_CASE(EXCEPTION_INVALID_HANDLE);

    case 0xE06D7363:
        return _T("C++ Exception");

    case MS_VC_EXCEPTION:
        return _T("Visual C++ exception");

    default:
        return _T("Unknown exception");
    }
}
#undef EX_CASE

void save_dump(LPEXCEPTION_POINTERS exceptionInfo)
{
    TCHAR timeStr[255];
    time_t t = time(NULL);
    tm tM ;
    localtime_s(&tM, &t);
    _tcsftime(timeStr, 255, _T("CrashDump_%d_%m_%Y_%H_%M_%S.dmp"), &tM);
    HANDLE hFile = CreateFile(
                                timeStr/*_T("dump.dmp")*/,
                                GENERIC_WRITE | GENERIC_READ,
                                0,
                                NULL,
                                CREATE_ALWAYS,
                                0,
                                NULL
                              );

    if(hFile != NULL)
    {
        MINIDUMP_EXCEPTION_INFORMATION info;
        info.ThreadId           = GetCurrentThreadId();
        info.ExceptionPointers  = exceptionInfo;
        info.ClientPointers     = TRUE;

        MiniDumpWriteDump(
                            GetCurrentProcess(),
                            GetCurrentProcessId(),
                            hFile,
                            MiniDumpWithIndirectlyReferencedMemory,
                            &info,
                            NULL,
                            NULL
                         );

        CloseHandle(hFile);
    }
}

void printStack( void )
{
     unsigned int   i;
     void         * stack[ 100 ];
     unsigned short frames;
     SYMBOL_INFO  * symbol;
     HANDLE         process;

     process = GetCurrentProcess();

     SymInitialize( process, NULL, TRUE );

     frames               = CaptureStackBackTrace( 0, 62, stack, NULL );
     symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
     symbol->MaxNameLen   = 255;
     symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

     for( i = 0; i < frames; i++ )
     {
         SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );
         printf( "%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address );
     }

     free( symbol );
}

void save_stack()
{
    TCHAR timeStr[255];
    time_t t = time(NULL);
    tm tM ;
    localtime_s(&tM, &t);
    _tcsftime(timeStr, 255, _T("Stack_%d_%m_%Y_%H_%M_%S.txt"), &tM);
    HANDLE hFile = CreateFile(
                                timeStr/*_T("dump.dmp")*/,
                                GENERIC_WRITE | GENERIC_READ,
                                0,
                                NULL,
                                CREATE_ALWAYS,
                                0,
                                NULL
                              );
    if(hFile != NULL)
    {
        unsigned int   i;
        void         * stack[ 100 ];
        unsigned short frames;
        SYMBOL_INFO  * symbol;
        HANDLE         process;
        char message[1024];

        process = GetCurrentProcess();

        SymInitialize( process, NULL, TRUE );

        frames               = CaptureStackBackTrace( 0, 62, stack, NULL );
        symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
        symbol->MaxNameLen   = 255;
        symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

        for( i = 0; i < frames; i++ )
        {
            SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );
            sprintf_s<1024>(message,
                              "%i: %s - 0x%0X\n", frames - i - 1,
                              symbol->Name, symbol->Address
                              );
            DWORD dwBytesWritten;
            WriteFile(hFile, message, strlen(message), &dwBytesWritten, NULL);
        }

        free( symbol );

        CloseHandle(hFile);
    }
}

HMODULE GetExceptionModule(LPVOID address, LPTSTR moduleName /* must support MAX_PATH characters */)
{
    HMODULE moduleList[1024];
    DWORD sizeNeeded = 0;
    if(FALSE == EnumProcessModules(GetCurrentProcess(), moduleList, 1024, &sizeNeeded) || sizeNeeded < sizeof(HMODULE))
        return NULL;

    int curModule = -1;
    for(DWORD i = 0; i < (sizeNeeded / sizeof(HMODULE)); ++i)
    {
        if((DWORD)moduleList[i] < (DWORD)address)
        {
            if(curModule == -1)
                curModule = i;
            else
            {
                if((DWORD)moduleList[curModule] < (DWORD)moduleList[i])
                 curModule = i;
            }
        }
    }

    if(curModule == -1)
        return NULL;

    if(!GetModuleFileName(moduleList[curModule], moduleName, MAX_PATH))
        return NULL;

    return moduleList[curModule];
}

LONG WINAPI UnhandledException(LPEXCEPTION_POINTERS exceptionInfo)
{
    TCHAR message[MAX_PATH + 255];
    TCHAR module[MAX_PATH];
    TCHAR* moduleName = NULL;
    if(GetExceptionModule(exceptionInfo->ExceptionRecord->ExceptionAddress, module))
        moduleName = module;
    else
        moduleName = _T("Unknown module!");

    _stprintf_s<MAX_PATH + 255>(message,
    _T("An exception has occured which was not handled!\nCode: %s\nModule: %s"),
    GetExceptionName(exceptionInfo->ExceptionRecord->ExceptionCode),
    moduleName
    );

    MessageBox(0, message, _T("Error!"), MB_OK);
    return EXCEPTION_EXECUTE_HANDLER;
}

void HandleAccessViolation(LPEXCEPTION_POINTERS info)
{
    TCHAR message[MAX_PATH + 512];
    TCHAR module[MAX_PATH];
    TCHAR* moduleName = NULL;
    if(GetExceptionModule(info->ExceptionRecord->ExceptionAddress, module))
        moduleName = module;
    else
        moduleName = _T("Unknown module!");

    DWORD codeBase = (DWORD)GetModuleHandle(NULL);
    DWORD offset = (DWORD)info->ExceptionRecord->ExceptionAddress - codeBase;

    TCHAR* accessType = NULL;
    switch(info->ExceptionRecord->ExceptionInformation[0])
        {
        case 0:
        accessType = _T("Read");
        break;
        case 1:
        accessType = _T("Write");
        break;
        case 2:
        accessType = _T("Execute");
        break;
        default:
        accessType = _T("Unknown");
        break;
        }

    _stprintf_s<MAX_PATH + 512>(message,
                                _T("An exception has occured which was not handled!\nCode: %s\nModule: %s\nThe thread %u tried to %s memory at address 0x%08X which is inaccessible!\nOffset: 0x%08X\nCodebase: 0x%08X"),
                                GetExceptionName(info->ExceptionRecord->ExceptionCode),
                                moduleName,
                                GetCurrentThreadId(),
                                accessType,
                                info->ExceptionRecord->ExceptionInformation[1],
                                offset,
                                codeBase
                                );

    MessageBox(0, message, _T("Error!"), MB_OK);
}

void HandleCommonException(LPEXCEPTION_POINTERS info)
{
    TCHAR message[MAX_PATH + 512];
    TCHAR module[MAX_PATH];
    TCHAR* moduleName = NULL;
    if(GetExceptionModule(info->ExceptionRecord->ExceptionAddress, module))
        moduleName = module;
    else
        moduleName = _T("Unknown module!");

    DWORD codeBase = (DWORD)GetModuleHandle(NULL);
    DWORD offset = (DWORD)info->ExceptionRecord->ExceptionAddress - codeBase;

    _stprintf_s<MAX_PATH + 512>(message,
                                _T("An exception has occured which was not handled!\nCode: %s\nModule: %s\nOffset: 0x%08X\nCodebase: 0x%08X"),
                                GetExceptionName(info->ExceptionRecord->ExceptionCode),
                                moduleName,
                                offset,
                                codeBase
                                );

    MessageBox(0, message, _T("Error!"), MB_OK);
}

LONG WINAPI UnhandledException2(LPEXCEPTION_POINTERS exceptionInfo)
{
    // Visual C++ exception
    if(exceptionInfo->ExceptionRecord->ExceptionCode == MS_VC_EXCEPTION)
        return EXCEPTION_EXECUTE_HANDLER;

    save_dump(exceptionInfo);
    save_stack();
    switch(exceptionInfo->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            HandleAccessViolation(exceptionInfo);
        break;
        default:
            HandleCommonException(exceptionInfo);
        break;
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

