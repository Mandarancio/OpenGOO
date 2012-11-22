/*******************************************************************************
*  file    : consoleappender.cpp
*  created : 22.11.2012
*  author  : 
*******************************************************************************/

#include "consoleappender.h"
#include "loggerevent.h"

void ConsoleAppender::write (const LoggerEvent &event)
{
    mutex.lock();
    QTextStream out(stream);
    streamWrite(out, event);
    mutex.unlock();
}

ConsoleAppender::ConsoleAppender (int level, FILE *stream, const QString &format) :
LoggerAppender(format, level)
{
    this->stream = stream;
}
