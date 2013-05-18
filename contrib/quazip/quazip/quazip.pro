TEMPLATE  = lib
TARGET    = quazip

# QuaZip version = 0.5.1
#!win32{VERSION=0.5.1}

include(../../../libs/libsworkbench.pri)
include(../quazip_dependencies.pri)

win32{
  #LIBS *= -L$${SOURCES_CONTRIBS_PATH}/quazip
}

LIBS += -lz

DEPENDPATH += .
INCLUDEPATH += .

DEFINES += QUAZIP_BUILD
CONFIG(staticlib): DEFINES += QUAZIP_STATIC

CONFIG(dontbuildquazip) {
  message(Using system libquazip0 package)
  LIBS+=-lquazip
} else {
  message(Building libquazip)
  include(quazip.pri)
}

# Input
HEADERS += \
    global.h

SOURCES += global.cpp
