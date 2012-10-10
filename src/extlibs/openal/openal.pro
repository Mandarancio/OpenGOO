

QT       -= qt


DESTDIR = ../libs
TEMPLATE = lib
CONFIG += dll

CONFIG(debug, debug|release) {
TARGET      = openald
OBJECTS_DIR = build/debug
MOC_DIR = build/debug
}
else{
TARGET      = openal
OBJECTS_DIR = build/release
MOC_DIR = build/release
QMAKE_CFLAGS_RELEASE += /MD
#QMAKE_LFLAGS += /NODEFAULTLIB
}

LIBS += -lwinmm -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32
DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE
DEFINES += AL_ALEXT_PROTOTYPES AL_BUILD_LIBRARY
DEFINES += strcasecmp=_stricmp strncasecmp=_strnicmp snprintf=_snprintf isfinite=_finite isnan=_isnan

INCLUDEPATH +=  ./ \
                ./openal-soft-1.14/include \
                ./openal-soft-1.14/include/AL \
                ./openal-soft-1.14/OpenAl32/include

SOURCES += \
    openal-soft-1.14/Alc/backends/winmm.c \
    openal-soft-1.14/Alc/backends/wave.c \
    openal-soft-1.14/Alc/backends/null.c \
    openal-soft-1.14/Alc/backends/mmdevapi.c \
    openal-soft-1.14/Alc/backends/loopback.c \
    openal-soft-1.14/Alc/backends/dsound.c

SOURCES += \
    openal-soft-1.14/Alc/panning.c \
    openal-soft-1.14/Alc/mixer.c \
    openal-soft-1.14/Alc/hrtf.c \
    openal-soft-1.14/Alc/helpers.c \
    openal-soft-1.14/Alc/bs2b.c \
    openal-soft-1.14/Alc/ALu.c \
    openal-soft-1.14/Alc/alcThread.c \
    openal-soft-1.14/Alc/alcRing.c \
    openal-soft-1.14/Alc/alcReverb.c \
    openal-soft-1.14/Alc/alcModulator.c \
    openal-soft-1.14/Alc/alcEcho.c \
    openal-soft-1.14/Alc/alcDedicated.c \
    openal-soft-1.14/Alc/alcConfig.c \
    openal-soft-1.14/Alc/ALc.c

HEADERS += \
    openal-soft-1.14/OpenAL32/Include/bs2b.h \
    openal-soft-1.14/OpenAL32/Include/alu.h \
    openal-soft-1.14/OpenAL32/Include/alThunk.h \
    openal-soft-1.14/OpenAL32/Include/alState.h \
    openal-soft-1.14/OpenAL32/Include/alSource.h \
    openal-soft-1.14/OpenAL32/Include/alMain.h \
    openal-soft-1.14/OpenAL32/Include/alListener.h \
    openal-soft-1.14/OpenAL32/Include/alFilter.h \
    openal-soft-1.14/OpenAL32/Include/alError.h \
    openal-soft-1.14/OpenAL32/Include/alEffect.h \
    openal-soft-1.14/OpenAL32/Include/alBuffer.h \
    openal-soft-1.14/OpenAL32/Include/alAuxEffectSlot.h

SOURCES += \
    openal-soft-1.14/OpenAL32/alThunk.c \
    openal-soft-1.14/OpenAL32/alState.c \
    openal-soft-1.14/OpenAL32/alSource.c \
    openal-soft-1.14/OpenAL32/alListener.c \
    openal-soft-1.14/OpenAL32/alFilter.c \
    openal-soft-1.14/OpenAL32/alExtension.c \
    openal-soft-1.14/OpenAL32/alError.c \
    openal-soft-1.14/OpenAL32/alEffect.c \
    openal-soft-1.14/OpenAL32/alBuffer.c \
    openal-soft-1.14/OpenAL32/alAuxEffectSlot.c

#SOURCES += \
#    openal-soft-1.14/utils/openal-info.c \
#    openal-soft-1.14/utils/makehrtf.c

HEADERS += \
    config.h

HEADERS += \
    openal-soft-1.14/OpenAL32/Include/bs2b.h \
    openal-soft-1.14/OpenAL32/Include/alu.h \
    openal-soft-1.14/OpenAL32/Include/alThunk.h \
    openal-soft-1.14/OpenAL32/Include/alState.h \
    openal-soft-1.14/OpenAL32/Include/alSource.h \
    openal-soft-1.14/OpenAL32/Include/alMain.h \
    openal-soft-1.14/OpenAL32/Include/alListener.h \
    openal-soft-1.14/OpenAL32/Include/alFilter.h \
    openal-soft-1.14/OpenAL32/Include/alError.h \
    openal-soft-1.14/OpenAL32/Include/alEffect.h \
    openal-soft-1.14/OpenAL32/Include/alBuffer.h \
    openal-soft-1.14/OpenAL32/Include/alAuxEffectSlot.h

HEADERS += \
    openal-soft-1.14/include/AL/efx-presets.h \
    openal-soft-1.14/include/AL/efx-creative.h \
    openal-soft-1.14/include/AL/efx.h \
    openal-soft-1.14/include/AL/alext.h \
    openal-soft-1.14/include/AL/alc.h \
    openal-soft-1.14/include/AL/al.h






