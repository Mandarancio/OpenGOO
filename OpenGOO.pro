QT       += core gui opengl xml

TARGET = OpenGOO
TEMPLATE = app
#ATTENCTION
#Lib Box2D must be in system folder and also the header files of it!
unix|win32: LIBS=-lBox2D

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
    svglevelloader.h


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
    object.cpp \
    stickylink.cpp \
    removablegoo.cpp \
    balloongoo.cpp\
    levelselector.cpp \
    svglevelloader.cpp


##FOR TOUCHPAD COMPILE
#!win32: {
#message(assume qws)
## Set the runtime path to a relative directory w.r.t. the location of the executable
#LIBS += -Wl,-rpath,"'\$$ORIGIN/qt'" -L/opt/PalmPDK/device/lib -lpdl -lSDL
#INCLUDEPATH += /opt/PalmPDK/include/SDL
#}
