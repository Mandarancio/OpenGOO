QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

DESTDIR = ../../
TEMPLATE = app

CONFIG(debug, debug|release) {
TARGET      = OpenGOO
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
INCLUDEPATH += ../opengoo/src
INCLUDEPATH += ../libs/logger/src
INCLUDEPATH += src/GameEngine
INCLUDEPATH += src/GameConfiguration
INCLUDEPATH += src/include

CONFIG(debug, debug|release) {
LIBS += -L../libs/lib -lloggerd
}
else {
LIBS += -L../libs/lib -llogger
}

LIBS += -L../extlibs/libs

!win32{
LIBS += -lBox2D
freebsd-g++|freebsd-clang {
LIBS += -lexecinfo
}
}

win32:{
DEFINES += _UNICODE DONT_GRAB_MOUSE
LIBS += -lDbgHelp -lAdvapi32 -lpsapi -lUser32


INCLUDEPATH += .
CONFIG += console


CONFIG(debug, debug|release) {
LIBS += -lBox2Dd
}
else {
LIBS += -lBox2D
QMAKE_CXXFLAGS += /Z7 /DEBUG  /Oy-
QMAKE_LFLAGS   += /DEBUG
}

#  Backtrace on win32
HEADERS += src/backtracer_win32.h
SOURCES += src/backtracer_win32.cpp
}

OTHER_FILES += \
    README \
    ../README \
    ../README.md \

HEADERS += \
    src/opengoo.h \
    src/flags.h \
    src/physics.h \
    src/circle.h \
    src/og_world.h \
    src/og_camera.h \
    src/og_button.h \
    src/og_sprite.h \
    src/og_windowcamera.h \        
    src/og_strand.h \
    src/og_ball.h \
    src/og_circle.h \
    src/og_ibody.h \
    src/og_rectangle.h \
    src/og_line.h \
    src/og_userdata.h \
    src/og_uibutton.h \
    src/og_ui.h \
    src/og_data.h \
    src/og_uiscene.h \
    src/og_event.h \
    src/og_poi.h \
    src/og_fpscounter.h \
    src/og_utils.h \
    src/og_ipipe.h \
    src/og_pipe.h \
    src/og_behavior.h \
    src/og_walk.h \
    src/og_climb.h \
    src/og_fly.h \
    src/og_staticbody.h \
    src/og_types.h \
    src/exitsensor.h \
    src/exit.h \
    src/GameEngine/og_resourcemanager.h \
    src/og_dispatcher.h \
    src/og_eventhandler.h \
    src/eventhandlers.h \
    src/handler_exit.h \
    src/handler_continue.h \
    src/progresswindow.h \
    src/og_uiwindow.h \
    src/continuebutton.h \
    src/og_uilabel.h \
    src/og_uipushbutton.h \
    src/continuebutton2.h


SOURCES += \
    src/main.cpp \
    src/flags.cpp \
    src/opengoo.cpp \
    src/physics.cpp \
    src/og_world.cpp \
    src/visualdebug.cpp \
    src/og_windowcamera.cpp \
    src/og_strand.cpp \
    src/og_ball.cpp \
    src/og_circle.cpp \
    src/og_ibody.cpp \
    src/og_rectangle.cpp \
    src/og_line.cpp \
    src/og_uibutton.cpp \
    src/og_ui.cpp \
    src/og_data.cpp \
    src/og_uiscene.cpp \
    src/og_event.cpp \
    src/og_camera.cpp \
    src/og_fpscounter.cpp \
    src/og_utils.cpp \
    src/og_pipe.cpp \   
    src/og_behavior.cpp \
    src/og_walk.cpp \
    src/og_climb.cpp \
    src/og_fly.cpp \
    src/og_staticbody.cpp \
    src/exitsensor.cpp \
    src/exit.cpp \
    src/GameEngine/og_resourcemanager.cpp \
    src/og_dispatcher.cpp \
    src/progresswindow.cpp \
    src/og_uiwindow.cpp \
    src/continuebutton.cpp \
    src/og_uipushbutton.cpp \
    src/continuebutton2.cpp


# Game engine
SOURCES += \
    src/GameEngine/og_window.cpp \
    src/GameEngine/og_gameengine.cpp \
    src/GameEngine/og_videomode.cpp \
    src/GameEngine/og_videomode_native.cpp \


HEADERS += \
    src/GameEngine/og_window.h \
    src/GameEngine/og_gameengine.h \
    src/GameEngine/og_videomode.h \
    src/GameEngine/og_videomode_native.h \
    src/GameEngine/og_game.h


# Physic engine
SOURCES += \
    src/PhysicsEngine/og_physicsshape.cpp \
    src/PhysicsEngine/og_physicsengine.cpp \
    src/PhysicsEngine/og_physicsbody.cpp \
    src/PhysicsEngine/og_pcircle.cpp \
    src/PhysicsEngine/og_circlesensor.cpp \
    src/PhysicsEngine/og_contactlistener.cpp


HEADERS += \
    src/PhysicsEngine/og_physicsshape.h \
    src/PhysicsEngine/og_physicsengine.h \
    src/PhysicsEngine/og_physicsbody.h \
    src/PhysicsEngine/og_physicsjoint.h \
    src/PhysicsEngine/og_pcircle.h \
    src/PhysicsEngine/debug.h \
    src/PhysicsEngine/common.h \
    src/PhysicsEngine/og_sensor.h \
    src/PhysicsEngine/og_contactlistener.h \
    src/PhysicsEngine/og_circlesensor.h

# OGLib
HEADERS += \
    src/OGLib/circle.h \
    src/OGLib/circlef.h \
    src/OGLib/point.h \
    src/OGLib/icamera.h \
    src/OGLib/pointf.h \
    src/OGLib/vector2d.h \
    src/OGLib/util.h \
    src/OGLib/size.h \
    src/OGLib/rect.h \
    src/OGLib/rectf.h


# Game configuration
SOURCES += \
    src/GameConfiguration/og_gameconfig.cpp \
    src/GameConfiguration/og_xmlconfig.cpp \
    src/GameConfiguration/og_resourceconfig.cpp \
    src/GameConfiguration/og_textconfig.cpp \
    src/GameConfiguration/og_sceneconfig.cpp \
    src/GameConfiguration/og_levelconfig.cpp \
    src/GameConfiguration/og_materialconfig.cpp \
    src/GameConfiguration/og_effectconfig.cpp \
    src/GameConfiguration/og_ballconfig.cpp \
    src/GameConfiguration/og_islandconfig.cpp \
    src/GameConfiguration/wog_level.cpp \
    src/GameConfiguration/wog_scene.cpp \
    src/GameConfiguration/wog_resources.cpp \
    src/GameConfiguration/wog_material.cpp \
    src/GameConfiguration/wog_text.cpp \
    src/GameConfiguration/wog_pipe.cpp


HEADERS += \
    src/GameConfiguration/og_gameconfig.h \
    src/GameConfiguration/og_xmlconfig.h \
    src/GameConfiguration/og_resourceconfig.h \
    src/GameConfiguration/og_textconfig.h \
    src/GameConfiguration/og_sceneconfig.h \
    src/GameConfiguration/og_levelconfig.h \
    src/GameConfiguration/og_materialconfig.h \
    src/GameConfiguration/og_effectconfig.h \
    src/GameConfiguration/og_ballconfig.h \
    src/GameConfiguration/og_islandconfig.h \
    src/GameConfiguration/wog_vobject.h \
    src/GameConfiguration/wog_scene.h \
    src/GameConfiguration/wog_pobject.h \
    src/GameConfiguration/wog_level.h \
    src/GameConfiguration/wog_resources.h \
    src/GameConfiguration/wog_text.h \
    src/GameConfiguration/wog_material.h \
    src/GameConfiguration/wog_effects.h \
    src/GameConfiguration/wog_ball.h \
    src/GameConfiguration/wog_island.h \
    src/GameConfiguration/wog_pipe.h \
    src/GameConfiguration/wog_exit.h \
    src/GameConfiguration/wog_circle.h


!win32{
HEADERS += src/backtracer.h
SOURCES += src/backtracer.cpp
}
