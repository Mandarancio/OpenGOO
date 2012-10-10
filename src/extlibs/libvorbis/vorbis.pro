

QT       -= qt


DESTDIR = ../libs
TEMPLATE = lib
CONFIG += dll

DEFINES += WIN32

LIBS += -L../libs

CONFIG(debug, debug|release) {
TARGET      = vorbisd
OBJECTS_DIR = build/debug
MOC_DIR = build/debug
LIBS += -loggd
}
else{
TARGET      = vorbis
OBJECTS_DIR = build/release
MOC_DIR = build/release
LIBS += -logg
}

QMAKE_LFLAGS += /DEF:".\\libvorbis-1.3.3\\win32\\vorbis.def"
DEFINES += _CRT_SECURE_NO_WARNINGS

INCLUDEPATH += ./libvorbis-1.3.3/include \
               ../libogg/libogg-1.3.0/include

HEADERS += \
    libvorbis-1.3.3/include/vorbis/vorbisenc.h \
    libvorbis-1.3.3/include/vorbis/codec.h\
    libvorbis-1.3.3/lib/books/coupled/res_books_stereo.h \
    libvorbis-1.3.3/lib/books/coupled/res_books_51.h\
    libvorbis-1.3.3/lib/books/floor/floor_books.h\
    libvorbis-1.3.3/lib/books/uncoupled/res_books_uncoupled.h\
    libvorbis-1.3.3/lib/modes/setup_X.h \
    libvorbis-1.3.3/lib/modes/setup_44u.h \
    libvorbis-1.3.3/lib/modes/setup_44p51.h \
    libvorbis-1.3.3/lib/modes/setup_44.h \
    libvorbis-1.3.3/lib/modes/setup_32.h \
    libvorbis-1.3.3/lib/modes/setup_22.h \
    libvorbis-1.3.3/lib/modes/setup_16.h \
    libvorbis-1.3.3/lib/modes/setup_11.h \
    libvorbis-1.3.3/lib/modes/setup_8.h \
    libvorbis-1.3.3/lib/modes/residue_44u.h \
    libvorbis-1.3.3/lib/modes/residue_44p51.h \
    libvorbis-1.3.3/lib/modes/residue_44.h \
    libvorbis-1.3.3/lib/modes/residue_16.h \
    libvorbis-1.3.3/lib/modes/residue_8.h \
    libvorbis-1.3.3/lib/modes/psych_44.h \
    libvorbis-1.3.3/lib/modes/psych_16.h \
    libvorbis-1.3.3/lib/modes/psych_11.h \
    libvorbis-1.3.3/lib/modes/psych_8.h \
    libvorbis-1.3.3/lib/modes/floor_all.h\
    libvorbis-1.3.3/lib/window.h \
    libvorbis-1.3.3/lib/smallft.h \
    libvorbis-1.3.3/lib/scales.h \
    libvorbis-1.3.3/lib/registry.h \
    libvorbis-1.3.3/lib/psy.h \
    libvorbis-1.3.3/lib/os.h \
    libvorbis-1.3.3/lib/misc.h \
    libvorbis-1.3.3/lib/mdct.h \
    libvorbis-1.3.3/lib/masking.h \
    libvorbis-1.3.3/lib/lsp.h \
    libvorbis-1.3.3/lib/lpc.h \
    libvorbis-1.3.3/lib/lookup_data.h \
    libvorbis-1.3.3/lib/lookup.h \
    libvorbis-1.3.3/lib/highlevel.h \
    libvorbis-1.3.3/lib/envelope.h \
    libvorbis-1.3.3/lib/codec_internal.h \
    libvorbis-1.3.3/lib/codebook.h \
    libvorbis-1.3.3/lib/bitrate.h \
    libvorbis-1.3.3/lib/backends.h

SOURCES += \
    libvorbis-1.3.3/lib/window.c \
    libvorbis-1.3.3/lib/vorbisenc.c \
    libvorbis-1.3.3/lib/synthesis.c \
    libvorbis-1.3.3/lib/smallft.c \
    libvorbis-1.3.3/lib/sharedbook.c \
    libvorbis-1.3.3/lib/res0.c \
    libvorbis-1.3.3/lib/registry.c \
    libvorbis-1.3.3/lib/psy.c \
    libvorbis-1.3.3/lib/mdct.c \
    libvorbis-1.3.3/lib/mapping0.c \
    libvorbis-1.3.3/lib/lsp.c \
    libvorbis-1.3.3/lib/lpc.c \
    libvorbis-1.3.3/lib/lookup.c \
    libvorbis-1.3.3/lib/info.c \
    libvorbis-1.3.3/lib/floor1.c \
    libvorbis-1.3.3/lib/floor0.c \
    libvorbis-1.3.3/lib/envelope.c \
    libvorbis-1.3.3/lib/codebook.c \
    libvorbis-1.3.3/lib/block.c \
    libvorbis-1.3.3/lib/bitrate.c \
    libvorbis-1.3.3/lib/barkmel.c \
    libvorbis-1.3.3/lib/analysis.c






