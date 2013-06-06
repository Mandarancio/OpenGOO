QT       -= core gui

TEMPLATE = lib
DESTDIR = ../libs

CONFIG += static warn_off

HEADERS += \
    src/AES.h

SOURCES += \
    src/AES.cpp

OTHER_FILES += \
    src/Readme.txt
