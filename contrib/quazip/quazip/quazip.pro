TEMPLATE  = lib
TARGET    = freemedforms-quazip-wrapper

# QuaZip version = 0.4.4
#!win32{VERSION=0.4.4}

include(../../../libs/libsworkbench.pri)
include(../quazip_dependencies.pri)

LIBS *= -lz

DEPENDPATH += .
INCLUDEPATH += .

DEFINES += QUAZIP_BUILD
CONFIG(staticlib): DEFINES += QUAZIP_STATIC

CONFIG(dontbuildquazip) {
  # On some OS (mainly Linux/FreeBSD) we do not want to build a duplicate of the quazip lib.
  # Using CONFIG+=dontbuildquazip will avoid to build duplicate code and will link the present lib
  # to the existing quazip
  message(Using system libquazip0 package)
  LIBS+=-lquazip
} else {
  # On other OS we need to build the quazip code.
  message(Building libquazip)
  include(quazip.pri)
}

# The wrapper code.
HEADERS += \
    global.h

SOURCES += \
    global.cpp
