QT       += core gui xml

TARGET = OpenGOO
TEMPLATE = app
#ATTENCTION
#Lib Box2D must be in system folder and also the header files of it!
!win32{
LIBS +=-lBox2D -lopenal -lalut  -lvorbisfile -lvorbis -logg
freebsd-g++|freebsd-clang {
LIBS += -lexecinfo
}
}

win32:{
DEFINES += _UNICODE
CONFIG(debug, debug|release) {
LIBS +=-lBox2Dd -lopenal32 -lalut
}
else {
LIBS +=-lBox2D -lopenal32 -lalut

#DEFINES += _DEBUG
QMAKE_CXXFLAGS += /Z7 /DEBUG  /Oy-
QMAKE_LFLAGS   += /DEBUG
}
LIBS += -lDbgHelp -lAdvapi32 -lpsapi -lUser32


INCLUDEPATH += .
CONFIG += console
#BOX2D_DIR - environment var must point to Box2D dir
INCLUDEPATH += $$(BOX2D_DIR)
LIBS +=      -L$$(BOX2D_DIR)/lib
#OPENAL_DIR
INCLUDEPATH += $$(OPENAL_DIR)/include
INCLUDEPATH += $$(OPENAL_DIR)/include/AL
LIBS +=      -L$$(OPENAL_DIR)/qtcreator-build
#FREEALUT_DIR
INCLUDEPATH += $$(FREEALUT_DIR)/include
LIBS +=      -L$$(FREEALUT_DIR)/lib

#  Backtrace on win32
HEADERS += backtracer_win32.h
SOURCES += backtracer_win32.cpp

}

OTHER_FILES += \
    level-svg.level \
    README \
    menu.index

HEADERS += \
    tools.h \
    thorn.h \
    target.h \
    mainwidget.h \
    level.h \
    joint.h \
    ground.h \
    goo.h \
    fixedgoo.h \
    dynamicgoo.h \
    collisionlistener.h \
    menu.h \
    object.h \
    stickylink.h \
    removablegoo.h \
    balloongoo.h\
    levelselector.h \
    svglevelloader.h \
    background.h \
    ropejoint.h \
    qb2draw.h \
    stickygoo.h \
    backgroundwidget.h \
    flags.h \
    mainmenu.h \
    introduction.h\
    soundsystem.h \
    publicclass.h


SOURCES += \
    tools.cpp \
    thorn.cpp \
    target.cpp \
    mainwidget.cpp \
    main.cpp \
    level.cpp \
    joint.cpp \
    ground.cpp \
    goo.cpp \
    fixedgoo.cpp \
    dynamicgoo.cpp \
    collisionlistener.cpp \
    menu.cpp \
    stickylink.cpp \
    removablegoo.cpp \
    balloongoo.cpp\
    levelselector.cpp \
    svglevelloader.cpp \
    background.cpp \
    ropejoint.cpp \
    qb2draw.cpp \
    stickygoo.cpp \
    backgroundwidget.cpp \
    flags.cpp \
    mainmenu.cpp \
    introduction.cpp\
    soundsystem.cpp \
    publicclass.cpp

!win32{
HEADERS += backtracer.h
SOURCES += backtracer.cpp
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
