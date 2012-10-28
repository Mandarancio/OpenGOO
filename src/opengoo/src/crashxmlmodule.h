#ifndef CRASHXMLMODULE_H
#define CRASHXMLMODULE_H

#include <QString>
#include <QUuid>

#include "xmlmodule.h"

class XmlModule;

/*!
* The CrashXmlModule class is an interface to OpenGooDst xml file, it inherits
* the opengoodst's xmlmodule, when the game crashes the module opens the reports' file
* and adds the crash information generating a unique Quuid.
* It is used by backtracer class.
*/


class CrashXmlModule {

public:
    CrashXmlModule(/*QString backtraceText*/);
    QUuid getUuid();

};

#endif // CRASHXMLMODULE_H
