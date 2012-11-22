#-------------------------------------------------
#
# Project created by QtCreator 2012-11-21T07:29:28
#
#-------------------------------------------------

QT       -= gui


TEMPLATE = lib
DESTDIR = ../lib

CONFIG += staticlib

CONFIG(debug, debug|release) {
TARGET      = loggerd
OBJECTS_DIR = build/debug
MOC_DIR     = build/debug
}
else{
TARGET      = logger
OBJECTS_DIR = build/release
MOC_DIR     = build/release
}

SOURCES += src/logger.cpp

HEADERS += src/logger.h


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

HEADERS += \
    src/loggerevent.h

SOURCES += \
    src/loggerevent.cpp

HEADERS += \
    src/loggerappender.h

SOURCES += \
    src/loggerappender.cpp

HEADERS += \
    src/consoleappender.h

SOURCES += \
    src/consoleappender.cpp
