#-------------------------------------------------
#
# Project created by QtCreator 2012-06-03T12:57:54
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenGooDst
DESTDIR = ../../
MOC_DIR = ./Build/Mocs
OBJECTS_DIR = ./Build/Objs
TEMPLATE = app

SOURCES += src/main.cpp \
        src/managergui.cpp \
    src/wizardgui_s0.cpp \
    src/wizardgui_s1.cpp \
    src/wizardgui_s2.cpp \
    src/wizardgui_s3.cpp \
    src/wizardmain.cpp \
    src/viewreport.cpp \
    src/sendtextreportform.cpp \
    src/report.cpp \
    src/architecture.cpp \
    src/xmlmodule.cpp \
    src/tablesmodel.cpp \
    src/customtableproxy.cpp

HEADERS  += src/managergui.h \
    src/wizardgui_s0.h \
    src/wizardgui_s1.h \
    src/wizardgui_s2.h \
    src/wizardgui_s3.h \
    src/wizardmain.h \
    src/viewreport.h \
    src/sendtextreportform.h \
    src/report.h \
    src/architecture.h \
    src/xmlmodule.h \
    src/tablesmodel.h \
    src/customtableproxy.h

FORMS    += src/managergui.ui \
    src/wizardgui_s0.ui \
    src/wizardgui_s1.ui \
    src/wizardgui_s2.ui \
    src/wizardgui_s3.ui \
    src/viewreport.ui \
    src/sendtextreportform.ui

TRANSLATIONS = ./src/res/lang/ogDst_it.ts \
               ./src/res/lang/ogDst_fr.ts \
               ./src/res/lang/ogDst_es.ts \
               ./src/res/lang/ogDst_de.ts

RESOURCES += \
    src/res.qrc























































