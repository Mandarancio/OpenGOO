#ifndef OG_UTILS_H
#define OG_UTILS_H

#include "og_gameconfig.h"
#include <consoleappender.h>

#include "OGLib/circlef.h"

struct OGUserData;

class OGContactListener;

namespace ogUtils
{

void ogBackTracer();

void ogLogger();

void ogMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);

OGConfig ogReadConfig(const QString & filename);

OGUserData* ogGetUserData(void* userdata);

OGContactListener* ogGetContactListener();
}

#endif // OG_UTILS_H
