

QT       -= qt


DESTDIR = ../../libs
TEMPLATE = lib
CONFIG += dll

LIBS += -L../../libs

CONFIG(debug, debug|release) {
TARGET      = vorbisfiled
OBJECTS_DIR = build/debug
MOC_DIR = build/debug
LIBS += -lvorbisd -loggd
}
else{
TARGET      = vorbisfile
OBJECTS_DIR = build/release
MOC_DIR = build/release

DEFINES += WIN32  NDEBUG _WINDOWS _USRDLL
LIBS += -lvorbis -logg
}

QMAKE_LFLAGS += /DEF:"..\\libvorbis-1.3.3\\win32\\vorbisfile.def"
DEFINES += _CRT_SECURE_NO_WARNINGS LIBVORBISFILE_EXPORTS

INCLUDEPATH += ../libvorbis-1.3.3/include \
               ../../libogg/libogg-1.3.0/include

HEADERS += \
    ../libvorbis-1.3.3/include/vorbis/vorbisfile.h

SOURCES += \
    ../libvorbis-1.3.3/lib/vorbisfile.c






