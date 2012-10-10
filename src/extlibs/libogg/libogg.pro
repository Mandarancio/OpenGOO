

QT       -= qt


DESTDIR = ../libs
TEMPLATE = lib
CONFIG += dll

CONFIG(debug, debug|release) {
TARGET      = oggd
OBJECTS_DIR = build/debug
MOC_DIR = build/debug
}
else{
TARGET      = ogg
OBJECTS_DIR = build/release
MOC_DIR = build/release

DEFINES += WIN32 NDEBUG _WINDOWS _USRDLL LIBOGG_EXPORTS
}

QMAKE_LFLAGS += /DEF:".\\libogg-1.3.0\\win32\\ogg.def"

DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += libogg-1.3.0/include

HEADERS += \
    libogg-1.3.0/include/ogg/os_types.h \
    libogg-1.3.0/include/ogg/ogg.h

SOURCES += \
    libogg-1.3.0/src/framing.c \
    libogg-1.3.0/src/bitwise.c

OTHER_FILES += \
    libogg-1.3.0/win32/ogg.def






