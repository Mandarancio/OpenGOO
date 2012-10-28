#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <QString>
#include <QObject>

/*!
The architecture object represents the computer where OpenGoo is run with some information.
The object is related to the generated reports and when something changes like the vga driver version, a new architecture
object is created.
*/

class Architecture {

public:
    Architecture(QString os, QString build, QString gameVer, QString arch, QString driverVer, QString vga, bool inUse);

    //Retrive data functions;
    QString getOs() const;
    QString getGameVer() const;
    QString getBuild() const;
    QString getArch() const;
    QString getVga() const;
    QString getDriverVer() const;
    bool getInUse() const;

private:
    QString os;
    QString build;
    QString gameVer;
    QString arch;
    QString driverVer;
    QString vga;
    bool inUse;
};

#endif // ARCHITECTURE_H
