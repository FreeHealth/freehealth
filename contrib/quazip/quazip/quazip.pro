# Qt5 cache system
cache()

TEMPLATE  = lib
TARGET    = freemedforms-quazip-wrapper

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

include(../../../libs/libsworkbench.pri)
include(../quazip_dependencies.pri)

DEPENDPATH += .
INCLUDEPATH += .

DEFINES += QUAZIP_BUILD
CONFIG(staticlib): DEFINES += QUAZIP_STATIC

CONFIG(dontbuildquazip) {
  # On Linux we do not want to build a duplicate of the quazip lib.
  # Using CONFIG+=dontbuildquazip (or using the buildspecs/optionalfeatures.pri config)
  # will avoid to build duplicate code and will link the present lib
  # to the existing quazip
  message(Using system libquazip0 package)
  LIBS+=-lquazip5
} else {
  # On other OS we need to build the quazip code.
  # QuaZip version = 0.5.1
  message(Building libquazip)
  include(quazip.pri)
}

# The wrapper code.
HEADERS += \
    global.h

SOURCES += \
    global.cpp
