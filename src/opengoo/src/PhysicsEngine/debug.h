#pragma once

#ifdef LOGGER
#include "logger.h"
#define DPRINT(msg) logDebug(msg)
#else
#include <QDebug>
#define DPRINT(msg) qDebug(msg)

#endif // LOGGER
