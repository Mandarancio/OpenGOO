#ifndef DEBUG_H
#define DEBUG_H

#ifdef LOGGER
#include "logger.h"
#define DPRINT(msg) logDebug(msg)

#else
#include <QDebug>
#define DPRINT(msg) qDebug(msg)

#endif // LOGGER

#endif // DEBUG_H
