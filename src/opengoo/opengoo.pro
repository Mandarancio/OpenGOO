QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = ../../
TEMPLATE = app

CONFIG(debug, debug|release) {
TARGET      = OpenGOOd
OBJECTS_DIR = build/debug
MOC_DIR = build/debug
}
else{
TARGET      = OpenGOO
OBJECTS_DIR = build/release
MOC_DIR = build/release
}


#ATTENCTION
#Lib Box2D must be in system folder and also the header files of it!
INCLUDEPATH += ../extlibs/box2d
INCLUDEPATH += ../extlibs/openal/openal-soft-1.14/include
INCLUDEPATH += ../extlibs/openal/openal-soft-1.14/include/AL
INCLUDEPATH += ../extlibs/libvorbis/libvorbis-1.3.3/include
INCLUDEPATH += ../extlibs/libogg/libogg-1.3.0/include
INCLUDEPATH += ../extlibs/freealut/freealut-1.1.0-src/include
INCLUDEPATH += ../opengoo/src
INCLUDEPATH += ../opengoodst/src
INCLUDEPATH += ../libs/logger/src
LIBS += -L../extlibs/libs -L../libs/lib

CONFIG(debug, debug|release) {
LIBS += -lloggerd
}
else {
LIBS += -llogger
}

!win32{
LIBS +=-lopenal -lalut  -lvorbisfile -lvorbis -logg -lBox2D
freebsd-g++|freebsd-clang {
LIBS += -lexecinfo
}
}

win32:{
DEFINES += _UNICODE DONT_GRAB_MOUSE
LIBS += -lDbgHelp -lAdvapi32 -lpsapi -lUser32


INCLUDEPATH += .
CONFIG += console
#OPENAL_DIR
INCLUDEPATH += $$(OPENAL_DIR)/include
INCLUDEPATH += $$(OPENAL_DIR)/include/AL
LIBS +=      -L$$(OPENAL_DIR)/qtcreator-build
#FREEALUT_DIR
INCLUDEPATH += $$(FREEALUT_DIR)/include
LIBS +=      -L$$(FREEALUT_DIR)/lib
#OGG_DIR
INCLUDEPATH += $$(OGG_DIR)/include
#VORBIS_DIR
INCLUDEPATH += $$(VORBIS_DIR)/include


CONFIG(debug, debug|release) {
#LIBS += -lopenal32 -lalut -llibogg -llibvorbis -llibvorbisfile
LIBS += -lopenald -lalutd  -lvorbisfiled -lvorbisd -loggd -lBox2dd

LIBS +=      -L$$(OGG_DIR)/win32/VS2010/Win32/Debug
LIBS +=      -L$$(VORBIS_DIR)/win32/VS2010/Win32/Debug
}
else {
#LIBS += -lopenal32 -lalut -llibogg -llibvorbis -llibvorbisfile
LIBS += -lopenal -lalut  -lvorbisfile -lvorbis -logg -lBox2d
#DEFINES += _DEBUG
QMAKE_CXXFLAGS += /Z7 /DEBUG  /Oy-
QMAKE_LFLAGS   += /DEBUG

LIBS +=      -L$$(OGG_DIR)/win32/VS2010/Win32/Release
LIBS +=      -L$$(VORBIS_DIR)/win32/VS2010/Win32/Release
}

#  Backtrace on win32
HEADERS += src/backtracer_win32.h
SOURCES += src/backtracer_win32.cpp

}

OTHER_FILES += \
    level-svg.level \
    README \
    menu.index \
    ../README \
    ../README.md

HEADERS += \
    src/tools.h \
    src/thorn.h \
    src/target.h \
    src/mainwidget.h \
    src/level.h \
    src/joint.h \
    src/ground.h \
    src/goo.h \
    src/fixedgoo.h \
    src/dynamicgoo.h \
    src/collisionlistener.h \
    src/menu.h \
    src/object.h \
    src/stickylink.h \
    src/removablegoo.h \
    src/balloongoo.h\
    src/levelselector.h \
    src/svglevelloader.h \
    src/background.h \
    src/ropejoint.h \
    src/qb2draw.h \
    src/stickygoo.h \
    src/backgroundwidget.h \
    src/flags.h \
    src/mainmenu.h \
    src/introduction.h\
    src/soundsystem.h \
    src/publicclass.h \
    src/options.h \
    src/crashxmlmodule.h \
    ../opengoodst/src/xmlmodule.h \
    src/og_types.h \
    src/og_oggfile.h \
    src/og_mplayer.h \
    src/og_ifile.h \
    src/og_alut.h \
    src/og_wavfile.h


SOURCES += \
    src/tools.cpp \
    src/thorn.cpp \
    src/target.cpp \
    src/mainwidget.cpp \
    src/main.cpp \
    src/level.cpp \
    src/joint.cpp \
    src/ground.cpp \
    src/goo.cpp \
    src/fixedgoo.cpp \
    src/dynamicgoo.cpp \
    src/collisionlistener.cpp \
    src/menu.cpp \
    src/stickylink.cpp \
    src/removablegoo.cpp \
    src/balloongoo.cpp\
    src/levelselector.cpp \
    src/svglevelloader.cpp \
    src/background.cpp \
    src/ropejoint.cpp \
    src/qb2draw.cpp \
    src/stickygoo.cpp \
    src/backgroundwidget.cpp \
    src/flags.cpp \
    src/mainmenu.cpp \
    src/introduction.cpp\
    src/soundsystem.cpp \
    src/publicclass.cpp \
    src/options.cpp \
    src/crashxmlmodule.cpp \
    ../opengoodst/src/xmlmodule.cpp \
    ../opengoodst/src/architecture.cpp \
    ../opengoodst/src/report.cpp \
    src/og_wavefile.cpp \
    src/og_oggfile.cpp \
    src/og_mplayer.cpp \
    src/og_alut.cpp

!win32{
HEADERS += src/backtracer.h
SOURCES += src/backtracer.cpp
}

#unix{
#    HEADERS+= qalsasound.h
#    SOURCES+= qalsasound.cpp
#}
##FOR TOUCHPAD COMPILE
#!win32: {
#message(assume qws)
## Set the runtime path to a relative directory w.r.t. the location of the executable
#LIBS += -Wl,-rpath,"'\$$ORIGIN/qt'" -L/opt/PalmPDK/device/lib -lpdl -lSDL
#INCLUDEPATH += /opt/PalmPDK/include/SDL
#}
