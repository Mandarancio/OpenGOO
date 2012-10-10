

QT       -= qt


DESTDIR = ../libs
TEMPLATE = lib
CONFIG += dll

LIBS += -L../libs

CONFIG(debug, debug|release) {
TARGET      = alutd
OBJECTS_DIR = build/debug
MOC_DIR = build/debug
LIBS += -lopenald
}
else{
TARGET      = alut
OBJECTS_DIR = build/release
MOC_DIR = build/release

LIBS += -lopenal
}

DEFINES += HAVE_WINDOWS_H HAVE_SLEEP HAVE__STAT ALUT_BUILD_LIBRARY _WIN32
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ../libvorbis/libvorbis-1.3.3/include \
               ../libogg/libogg-1.3.0/include \
               ./freealut-1.1.0-src/include \
               ../openal/openal-soft-1.14/include/AL


DEFINES += HAVE_STDINT_H

HEADERS += \
    freealut-1.1.0-src/include/AL/alut.h

HEADERS += \
    freealut-1.1.0-src/src/alutInternal.h

SOURCES += \
    freealut-1.1.0-src/src/alutWaveform.c \
    freealut-1.1.0-src/src/alutVersion.c \
    freealut-1.1.0-src/src/alutUtil.c \
    freealut-1.1.0-src/src/alutOutputStream.c \
    freealut-1.1.0-src/src/alutLoader.c \
    freealut-1.1.0-src/src/alutInputStream.c \
    freealut-1.1.0-src/src/alutInit.c \
    freealut-1.1.0-src/src/alutError.c \
    freealut-1.1.0-src/src/alutCodec.c \
    freealut-1.1.0-src/src/alutBufferData.c








