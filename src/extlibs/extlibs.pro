######################################################################
#
######################################################################

TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = box2d

win32:{
SUBDIRS += libogg \
          libvorbis \
          openal \
          freealut
}

