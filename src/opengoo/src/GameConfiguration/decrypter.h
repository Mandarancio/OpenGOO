#pragma once

#include <qsystemdetection.h>

#ifdef Q_OS_OSX
#include "xordecrypter.h"
typedef XorDecrypter Decrypter;
#else
#include "aesdecrypter.h"
typedef AesDecrypter Decrypter;
#endif

