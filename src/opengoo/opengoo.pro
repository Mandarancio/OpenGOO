QT += core gui xml opengl
CONFIG += c++11

DESTDIR = ../../
TEMPLATE = app

CONFIG(debug, debug|release) {
    TARGET = OpenGOO
    OBJECTS_DIR = build/debug
    MOC_DIR = build/debug
} else {
    TARGET = OpenGOO
    OBJECTS_DIR = build/release
    MOC_DIR = build/release
}

#ATTENCTION
#Lib Box2D must be in system folder and also the header files of it!
INCLUDEPATH += ../extlibs/box2d
INCLUDEPATH += ../opengoo/src
INCLUDEPATH += ../libs/logger/src
INCLUDEPATH += src/GameEngine
INCLUDEPATH += src/GameConfiguration
INCLUDEPATH += src/include

CONFIG(debug, debug|release) {
    LIBS += -L../libs/lib -lloggerd
} else {
    LIBS += -L../libs/lib -llogger
}

LIBS += -L../extlibs/libs

CONFIG(debug, debug|release) {
    LIBS += -lBox2Dd
} else {
    LIBS += -lBox2D
}

win32 {
    DEFINES += _UNICODE DONT_GRAB_MOUSE
    LIBS += -lDbgHelp -lAdvapi32 -lpsapi -lUser32

    INCLUDEPATH += .
    CONFIG += console

    msvc: {
        #  Backtrace on win32
        HEADERS += src/backtracer_win32.h
        SOURCES += src/backtracer_win32.cpp
        release: {
#            QMAKE_CXXFLAGS += /Z7 /DEBUG  /Oy-
#            QMAKE_LFLAGS   += /DEBUG
        }
    }
} else {
    freebsd-g++|freebsd-clang {
        LIBS += -lexecinfo
    }
}

HEADERS += \
    src/opengoo.h \
    src/flags.h \
    src/physics.h \
    src/circle.h \
    src/fpscounter.h \
    src/og_world.h \
    src/og_camera.h \
    src/og_button.h \
    src/og_sprite.h \
    src/og_windowcamera.h \
    src/og_strand.h \
    src/og_circle.h \
#    src/og_ibody.h \
    src/og_rectangle.h \
    src/og_line.h \
    src/og_userdata.h \
    src/og_uibutton.h \
    src/og_poi.h \
    src/og_fpscounter.h \
    src/og_utils.h \
    src/og_ipipe.h \
    src/og_behavior.h \
    src/og_walk.h \
    src/og_climb.h \
    src/og_fly.h \
    src/og_types.h \
#    src/progresswindow.h \
#    src/og_layer.h \
#    src/island.h \
#    src/retrymenu.h \
#    src/gamemenu.h \
#    src/level.h \
    src/uidata.h \
#    src/continuebutton.h \
    src/og_forcefield.h \
    src/ogapplication.h \
    src/ballsensor.h \
    src/entityfactory.h \
    src/gamedata.h \
    src/exiteventlistener.h \
    src/ijointbuilder.h \
    src/physicsenginefactory.h \
    src/sceneloader.h \
    src/animator.h \
    src/og_resourcemanager.h \
    src/resourcemanagerfactory.h \
    src/font.h \
    src/animation.h \
    src/sequentialanimationgroup.h \
    src/animationgroup.h \
    src/scaleanimation.h \
    src/translateanimation.h \
    src/rotationanimation.h \
    src/multisprite.h \
    src/circlesprite.h \
    src/rectsprite.h \
    src/linesprite.h \
    src/camzoomanimation.h \
    src/configloader.h

SOURCES += \
    src/main.cpp \
    src/flags.cpp \
    src/opengoo.cpp \
    src/physics.cpp \
    src/og_world.cpp \
    src/visualdebug.cpp \
    src/og_windowcamera.cpp \
    src/og_strand.cpp \
#    src/og_circle.cpp \
#    src/og_ibody.cpp \
#    src/og_rectangle.cpp \
#    src/og_line.cpp \
#    src/og_uibutton.cpp \
    src/og_camera.cpp \
    src/fpscounter.cpp \
    src/og_fpscounter.cpp \
    src/og_utils.cpp \
    src/og_behavior.cpp \
    src/og_walk.cpp \
    src/og_climb.cpp \
    src/og_fly.cpp \
#    src/progresswindow.cpp \
#    src/og_layer.cpp \
#    src/island.cpp \
#    src/retrymenu.cpp \
#    src/gamemenu.cpp \
#    src/level.cpp \
#    src/continuebutton.cpp \
    src/og_forcefield.cpp \
    src/ogapplication.cpp \
    src/ballsensor.cpp \
    src/og_sprite.cpp \
    src/entityfactory.cpp \
    src/sceneloader.cpp \
    src/animator.cpp \
    src/og_resourcemanager.cpp \
    src/resourcemanagerfactory.cpp

win32 {
    INCLUDEPATH += $$PWD/../extlibs/SFML-2.3.2/include
    DEPENDPATH += $$PWD/../extlibs/SFML-2.3.2/include
} else:unix {
    INCLUDEPATH += /usr/local/include
    DEPENDPATH += /usr/local/include
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../extlibs/SFML-2.3.2/lib/ -lsfml-audio -lsfml-system
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../extlibs/SFML-2.3.2/lib/ -lsfml-audio-d -lsfml-system-d
else:unix: LIBS += -L/usr/local/lib/ -lsfml-system -lsfml-audio

include(src/GameEngine/GameEngine.files)
include(src/PhysicsEngine/PhysicsEngine.files)
include(src/OGLib/OGLib.files)
include(src/GameConfiguration/GameConfiguration.files)
include(src/entities/entities.files)
include(src/SoundEngine/SoundEngine.files)

!win32 {
    HEADERS += src/backtracer.h
    SOURCES += src/backtracer.cpp
}

RESOURCES += \
    resources.qrc
