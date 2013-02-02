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
#INCLUDEPATH += ../extlibs/openal/openal-soft-1.14/include
#INCLUDEPATH += ../extlibs/openal/openal-soft-1.14/include/AL
#INCLUDEPATH += ../extlibs/libvorbis/libvorbis-1.3.3/include
#INCLUDEPATH += ../extlibs/libogg/libogg-1.3.0/include
#INCLUDEPATH += ../extlibs/freealut/freealut-1.1.0-src/include
INCLUDEPATH += ../opengoo/src
#INCLUDEPATH += ../opengoodst/src
INCLUDEPATH += ../libs/logger/src
INCLUDEPATH += src/GameEngine
INCLUDEPATH += src/PhysicsEngine
INCLUDEPATH += src/GameConfiguration

CONFIG(debug, debug|release) {
LIBS += -L../libs/lib -lloggerd
}
else {
LIBS += -L../libs/lib -llogger
}

LIBS += -L../extlibs/libs

!win32{
#LIBS += -lopenal -lvorbisfile -lvorbis -logg -lBox2D
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
##OPENAL_DIR
#INCLUDEPATH += $$(OPENAL_DIR)/include
#INCLUDEPATH += $$(OPENAL_DIR)/include/AL
#LIBS +=      -L$$(OPENAL_DIR)/qtcreator-build
##FREEALUT_DIR
#INCLUDEPATH += $$(FREEALUT_DIR)/include
#LIBS +=      -L$$(FREEALUT_DIR)/lib
##OGG_DIR
#INCLUDEPATH += $$(OGG_DIR)/include
##VORBIS_DIR
#INCLUDEPATH += $$(VORBIS_DIR)/include


CONFIG(debug, debug|release) {
#LIBS += -lopenal32 -lalut -llibogg -llibvorbis -llibvorbisfile
#LIBS += -lopenald -lvorbisfiled -lvorbisd -loggd -lBox2dd
LIBS += -lBox2Dd

#LIBS +=      -L$$(OGG_DIR)/win32/VS2010/Win32/Debug
#LIBS +=      -L$$(VORBIS_DIR)/win32/VS2010/Win32/Debug
}
else {
#LIBS += -lopenal32 -lalut -llibogg -llibvorbis -llibvorbisfile
#LIBS += -lopenal -lvorbisfile -lvorbis -logg -lBox2d
LIBS += -lBox2D
#DEFINES += _DEBUG
QMAKE_CXXFLAGS += /Z7 /DEBUG  /Oy-
QMAKE_LFLAGS   += /DEBUG

#LIBS +=      -L$$(OGG_DIR)/win32/VS2010/Win32/Release
#LIBS +=      -L$$(VORBIS_DIR)/win32/VS2010/Win32/Release
}

#  Backtrace on win32
HEADERS += src/backtracer_win32.h
SOURCES += src/backtracer_win32.cpp

}

OTHER_FILES += \
#    level-svg.level \
    README \
#    menu.index \
    ../README \
    ../README.md

HEADERS += \
#    src/tools.h \
#    src/thorn.h \
#    src/target.h \
#    src/mainwidget.h \
#    src/level.h \
#    src/joint.h \
#    src/ground.h \
#    src/goo.h \
#    src/fixedgoo.h \
#    src/dynamicgoo.h \
#    src/collisionlistener.h \
#    src/menu.h \
#    src/object.h \
#    src/stickylink.h \
#    src/removablegoo.h \
#    src/balloongoo.h\
#    src/levelselector.h \
#    src/svglevelloader.h \
#    src/background.h \
#    src/ropejoint.h \
#    src/qb2draw.h \
#    src/stickygoo.h \
#    src/backgroundwidget.h \
    src/flags.h \
#    src/mainmenu.h \
#    src/introduction.h\
#    src/soundsystem.h \
#    src/publicclass.h \
#    src/options.h \
#    src/crashxmlmodule.h \
#    ../opengoodst/src/xmlmodule.h \
#    src/og_types.h \
#    src/og_oggfile.h \
#    src/og_mplayer.h \
#    src/og_ifile.h \
#    src/og_alut.h \
#    src/og_wavfile.h \
    src/opengoo.h \     
    src/og_world.h \
    src/og_camera.h \
    src/og_button.h \
    src/og_sprite.h \
    src/og_windowcamera.h \        
    src/physics.h \


SOURCES += \
#    src/tools.cpp \
#    src/thorn.cpp \
#    src/target.cpp \
#    src/mainwidget.cpp \
#    src/level.cpp \
#    src/joint.cpp \
#    src/ground.cpp \
#    src/goo.cpp \
#    src/fixedgoo.cpp \
#    src/dynamicgoo.cpp \
#    src/collisionlistener.cpp \
#    src/menu.cpp \
#    src/stickylink.cpp \
#    src/removablegoo.cpp \
#    src/balloongoo.cpp\
#    src/levelselector.cpp \
#    src/svglevelloader.cpp \
#    src/background.cpp \
#    src/ropejoint.cpp \
#    src/qb2draw.cpp \
#    src/stickygoo.cpp \
#    src/backgroundwidget.cpp \
    src/flags.cpp \
#    src/mainmenu.cpp \
#    src/introduction.cpp\
#    src/soundsystem.cpp \
#    src/publicclass.cpp \
#    src/options.cpp \
#    src/crashxmlmodule.cpp \
#    ../opengoodst/src/xmlmodule.cpp \
#    ../opengoodst/src/architecture.cpp \
#    ../opengoodst/src/report.cpp \
#    src/og_wavefile.cpp \
#    src/og_oggfile.cpp \
#    src/og_mplayer.cpp \
#    src/og_alut.cpp \
    src/opengoo.cpp \
    src/og_world.cpp \
    src/visualdebug.cpp \
    src/og_windowcamera.cpp \
    src/physics.cpp \


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


# Physic engine
SOURCES += \
    src/PhysicsEngine/og_physicsshape.cpp \
    src/PhysicsEngine/og_physicsengine.cpp \
    src/PhysicsEngine/og_physicsbody.cpp \


HEADERS += \
    src/PhysicsEngine/og_physicsshape.h \
    src/PhysicsEngine/og_physicsengine.h \
    src/PhysicsEngine/og_physicsbody.h \
    src/PhysicsEngine/og_physicsjoint.h \
    src/PhysicsEngine/debug.h \
    src/PhysicsEngine/common.h \   


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



