#include "architecture.h"
#include <QDebug>

Architecture::Architecture(QString os, QString build, QString gameVer, QString arch, QString driverVer, QString vga, bool inUse)
{
    this->os = os;
    this->build = build;
    this->gameVer = gameVer;
    this->arch = arch;
    this->driverVer = driverVer;
    this->vga = vga;
    this->inUse = inUse;
}

QString Architecture::getOs() const {
    return os;
}

QString Architecture::getGameVer() const {
    return gameVer;
}

QString Architecture::getBuild() const {
    return build;
}

QString Architecture::getArch() const {
    return arch;
}

QString Architecture::getDriverVer() const {
    return driverVer;
}

QString Architecture::getVga() const {
    return vga;
}

bool Architecture::getInUse() const {
    return inUse;
}

