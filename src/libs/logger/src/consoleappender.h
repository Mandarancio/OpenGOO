/*******************************************************************************
*  file    : consoleappender.h
*  created : 22.11.2012
*  author  : 
*******************************************************************************/

#ifndef CONSOLEAPPENDER_H
#define CONSOLEAPPENDER_H

#include "loggerappender.h"

class ConsoleAppender: public LoggerAppender
{
    public:
        FILE *stream;

    public:
        void write (const LoggerEvent &event);
        ConsoleAppender (int level, FILE *stream, const QString &format);
};

#endif // CONSOLEAPPENDER_H
