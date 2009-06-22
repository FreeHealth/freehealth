#define all applications name
FREEMEDFORMS          = $$quote(FreeMedForms)
DRUGSINTERACTIONS     = $$quote(drugsinteractions)

# prepare package name // TARGET __MUST BE__ defined BEFORE this file is included
isEmpty(TARGET) {
    error("config.pri: You must provide a TARGET")
}

PACKAGE_TARGET             = $$quote($${TARGET})

# define some paths
PACKAGE_PWD	           = $$PWD
PACKAGE_DESTDIR            = $${PWD}/bin
PACKAGE_LIBS_SOURCES       = $${PWD}/libs
PACKAGE_LIBS_BIN           = $${PACKAGE_DESTDIR}/libs
PACKAGE_CONTRIBS           = $${PWD}/contrib
PACKAGE_BUILD_PATH         = $${PWD}/build
FMF_GLOBAL_RESOURCES       = $${PWD}/global_resources
FMF_RELEASE_PACKAGES_PATH  = $${PWD}/packages
PROTECTED_PATH             = $${PWD}/_protected
exists( $${PROTECTED_PATH} ){
    message( *** BUILDING FULL APPLICATION *** )
    DEFINES *= FULLAPPLICATION_BUILD
}

# temporary path for building

#include all SDK
INCLUDEPATH *= $${PACKAGE_CONTRIBS}/quazip \
               $${PWD}/freemedforms/sdk

# build mode
CONFIG     += qt warn_on thread x11 windows
mac:CONFIG *= x86 ppc

# define build paths
CONFIG( debug, debug|release ) {
  #Debug
  CONFIG      += console
  mac:CONFIG  -= ppc
  mac:RPATH_LIBS_BIN   = ../../../libs/
  else:RPATH_LIBS_BIN   = libs/
  unix{
      PACKAGE_TARGET   = $$quote($$join(PACKAGE_TARGET,,,_debug))
      FREEMEDFORMS     = $$quote($$join(FREEMEDFORMS,,,_debug))
  }else:{
      PACKAGE_TARGET   = $$quote($$join(PACKAGE_TARGET,,,_d))
      FREEMEDFORMS     = $$quote($$join(FREEMEDFORMS,,,_d))
  }
  message( Building $${PACKAGE_TARGET} in DEBUG )
  DEFINES  *= DEBUG
} else {
  #Release
  message( Building $${PACKAGE_TARGET} in RELEASE )
  RPATH_LIBS_BIN   = ../libs/
  DEFINES   *= RELEASE \
               QT_NO_DEBUG_OUTPUT \
               QT_NO_WARNING_OUTPUT \
               QT_NO_DEBUG
}

# define objects, ui, rcc building path
unix:OBJECTS_DIR   = $${PACKAGE_BUILD_PATH}/$${PACKAGE_TARGET}/.obj/unix
win32:OBJECTS_DIR  = $${PACKAGE_BUILD_PATH}/$${PACKAGE_TARGET}/.obj/win32
mac:OBJECTS_DIR    = $${PACKAGE_BUILD_PATH}/$${PACKAGE_TARGET}/.obj/mac
CONFIG(crosscompil):OBJECTS_DIR = $${PACKAGE_BUILD_PATH}/$${PACKAGE_TARGET}/.obj/win-x

UI_DIR	           = $${PACKAGE_BUILD_PATH}/.ui
MOC_DIR	           = $${PACKAGE_BUILD_PATH}/$${TARGET}/.moc
RCC_DIR	           = $${PACKAGE_BUILD_PATH}/$${TARGET}/.rcc

# define qome path related to the binary

mac{
     FMF_BIN_PATH       = $${PACKAGE_DESTDIR}/$${FREEMEDFORMS}.app/Contents/MacOs
     FMF_PLUGINS_DIR    = $${PACKAGE_DESTDIR}/$${FREEMEDFORMS}.app/Contents/plugins
} else:{
     FMF_PLUGINS_DIR   = $${PACKAGE_DESTDIR}/plugins
     FMF_BIN_PATH      = $${PACKAGE_DESTDIR}
}

LIBS *= -L$${PACKAGE_LIBS_BIN}

# define some usefull values
QMAKE_TARGET_COMPANY       = "Eric MAEKER and the FreeMedForms Team"
QMAKE_TARGET_PRODUCT       = $${TARGET}
QMAKE_TARGET_DESCRIPTION   = "Crossplatform Medical Forms Generator"
QMAKE_TARGET_COPYRIGHT     = "Copyright (C) 2008-2009 Eric MAEKER, MD"
PACKAGE_DOMAIN             = "http://www.ericmaeker.fr/FreeMedForms"

# define variable for source code
DEFINES	*= "PACKAGE_NAME=\"\\\"$${QMAKE_TARGET_PRODUCT}\\\"\"" \
           "PACKAGE_VERSION=\"\\\"$${PACKAGE_VERSION}\\\"\"" \
           "PACKAGE_DOMAIN=\"\\\"$${PACKAGE_DOMAIN}\\\"\"" \
           "PACKAGE_COPYRIGHTS=\"\\\"$${QMAKE_TARGET_COPYRIGHT}\\\"\"" \
           "FMF_BIN_PATH=\"\\\"$${FMF_BIN_PATH}\\\"\"" \
           "FMF_GLOBAL_RESOURCES=\"\\\"$${FMF_GLOBAL_RESOURCES}\\\"\"" \
           "FMF_PLUGINS_DIR=\"\\\"$${FMF_PLUGINS_DIR}\\\"\""

TARGET   = $${PACKAGE_TARGET}
DESTDIR  = $${PACKAGE_DESTDIR}
