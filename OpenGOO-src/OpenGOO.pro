QT       += core gui xml

TARGET = OpenGOO
DESTDIR = ../
MOC_DIR = Build/Mocs
OBJECTS_DIR = Build/Objs
TEMPLATE = app
#ATTENCTION
#Lib Box2D must be in system folder and also the header files of it!
!win32{
LIBS +=-lopenal -lalut  -lvorbisfile -lvorbis -logg
freebsd-g++|freebsd-clang {
LIBS += -lexecinfo
}
}

win32:{
DEFINES += _UNICODE
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
LIBS += -lopenal32 -lalut -llibogg -llibvorbis -llibvorbisfile

LIBS +=      -L$$(OGG_DIR)/win32/VS2010/Win32/Debug
LIBS +=      -L$$(VORBIS_DIR)/win32/VS2010/Win32/Debug
}
else {
LIBS += -lopenal32 -lalut -llibogg -llibvorbis -llibvorbisfile
#DEFINES += _DEBUG
QMAKE_CXXFLAGS += /Z7 /DEBUG  /Oy-
QMAKE_LFLAGS   += /DEBUG

LIBS +=      -L$$(OGG_DIR)/win32/VS2010/Win32/Release
LIBS +=      -L$$(VORBIS_DIR)/win32/VS2010/Win32/Release
}

#  Backtrace on win32
HEADERS += backtracer_win32.h
SOURCES += backtracer_win32.cpp

}

OTHER_FILES += \
    level-svg.level \
    README \
    menu.index \
    ../README \
    ../README.md

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
    publicclass.h \
    playsoundthread.h \
    lib/Box2D/Collision/b2TimeOfImpact.h \
    lib/Box2D/Collision/b2DynamicTree.h \
    lib/Box2D/Collision/b2Distance.h \
    lib/Box2D/Collision/b2Collision.h \
    lib/Box2D/Collision/b2BroadPhase.h \
    lib/Box2D/Box2D.h \
    lib/Box2D/Rope/b2Rope.h \
    lib/Box2D/Dynamics/b2WorldCallbacks.h \
    lib/Box2D/Dynamics/b2World.h \
    lib/Box2D/Dynamics/b2TimeStep.h \
    lib/Box2D/Dynamics/b2Island.h \
    lib/Box2D/Dynamics/b2Fixture.h \
    lib/Box2D/Dynamics/b2ContactManager.h \
    lib/Box2D/Dynamics/b2Body.h \
    lib/Box2D/Dynamics/Joints/b2WheelJoint.h \
    lib/Box2D/Dynamics/Joints/b2WeldJoint.h \
    lib/Box2D/Dynamics/Joints/b2RopeJoint.h \
    lib/Box2D/Dynamics/Joints/b2RevoluteJoint.h \
    lib/Box2D/Dynamics/Joints/b2PulleyJoint.h \
    lib/Box2D/Dynamics/Joints/b2PrismaticJoint.h \
    lib/Box2D/Dynamics/Joints/b2MouseJoint.h \
    lib/Box2D/Dynamics/Joints/b2Joint.h \
    lib/Box2D/Dynamics/Joints/b2GearJoint.h \
    lib/Box2D/Dynamics/Joints/b2FrictionJoint.h \
    lib/Box2D/Dynamics/Joints/b2DistanceJoint.h \
    lib/Box2D/Dynamics/Contacts/b2PolygonContact.h \
    lib/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.h \
    lib/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.h \
    lib/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.h \
    lib/Box2D/Dynamics/Contacts/b2ContactSolver.h \
    lib/Box2D/Dynamics/Contacts/b2Contact.h \
    lib/Box2D/Dynamics/Contacts/b2CircleContact.h \
    lib/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.h \
    lib/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.h \
    lib/Box2D/Collision/Shapes/b2Shape.h \
    lib/Box2D/Collision/Shapes/b2PolygonShape.h \
    lib/Box2D/Collision/Shapes/b2EdgeShape.h \
    lib/Box2D/Collision/Shapes/b2CircleShape.h \
    lib/Box2D/Collision/Shapes/b2ChainShape.h \
    lib/Box2D/Common/b2Timer.h \
    lib/Box2D/Common/b2StackAllocator.h \
    lib/Box2D/Common/b2Settings.h \
    lib/Box2D/Common/b2Math.h \
    lib/Box2D/Common/b2GrowableStack.h \
    lib/Box2D/Common/b2Draw.h \
    lib/Box2D/Common/b2BlockAllocator.h


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
    publicclass.cpp \
    playsoundthread.cpp \
    lib/Box2D/Collision/b2TimeOfImpact.cpp \
    lib/Box2D/Collision/b2DynamicTree.cpp \
    lib/Box2D/Collision/b2Distance.cpp \
    lib/Box2D/Collision/b2Collision.cpp \
    lib/Box2D/Collision/b2CollidePolygon.cpp \
    lib/Box2D/Collision/b2CollideEdge.cpp \
    lib/Box2D/Collision/b2CollideCircle.cpp \
    lib/Box2D/Collision/b2BroadPhase.cpp \
    lib/Box2D/Rope/b2Rope.cpp \
    lib/Box2D/Dynamics/b2WorldCallbacks.cpp \
    lib/Box2D/Dynamics/b2World.cpp \
    lib/Box2D/Dynamics/b2Island.cpp \
    lib/Box2D/Dynamics/b2Fixture.cpp \
    lib/Box2D/Dynamics/b2ContactManager.cpp \
    lib/Box2D/Dynamics/b2Body.cpp \
    lib/Box2D/Dynamics/Joints/b2WheelJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2WeldJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2RopeJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2PulleyJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2MouseJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2Joint.cpp \
    lib/Box2D/Dynamics/Joints/b2GearJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2FrictionJoint.cpp \
    lib/Box2D/Dynamics/Joints/b2DistanceJoint.cpp \
    lib/Box2D/Dynamics/Contacts/b2PolygonContact.cpp \
    lib/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
    lib/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
    lib/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
    lib/Box2D/Dynamics/Contacts/b2ContactSolver.cpp \
    lib/Box2D/Dynamics/Contacts/b2Contact.cpp \
    lib/Box2D/Dynamics/Contacts/b2CircleContact.cpp \
    lib/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
    lib/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
    lib/Box2D/Collision/Shapes/b2PolygonShape.cpp \
    lib/Box2D/Collision/Shapes/b2EdgeShape.cpp \
    lib/Box2D/Collision/Shapes/b2CircleShape.cpp \
    lib/Box2D/Collision/Shapes/b2ChainShape.cpp \
    lib/Box2D/Common/b2Timer.cpp \
    lib/Box2D/Common/b2StackAllocator.cpp \
    lib/Box2D/Common/b2Settings.cpp \
    lib/Box2D/Common/b2Math.cpp \
    lib/Box2D/Common/b2Draw.cpp \
    lib/Box2D/Common/b2BlockAllocator.cpp

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
