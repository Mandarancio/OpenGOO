#ifndef OG_UTILS_H
#define OG_UTILS_H

#include "og_gameconfig.h"
#include <consoleappender.h>

namespace ogUtils
{
void ogBackTracer();

void ogLogger();

void ogMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);

OGConfig ogReadConfig(const QString & filename);
}

#endif // OG_UTILS_H
