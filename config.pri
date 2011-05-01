# prepare package name // TARGET __MUST BE__ defined BEFORE this file is included
isEmpty(TARGET) {
    error("config.pri: You must provide a TARGET")
}
# include the generic cofiguration file (define some paths)
SOURCES_ROOT_PATH        = $${PWD}
include(buildspecs/config_paths.pri)
include(buildspecs/svnversion.pri)

macx:include(buildspecs/config_mac.pri)
else:linux*:include(buildspecs/config_linux.pri)
else:freebsd*:include(buildspecs/config_freebsd.pri)
else:win32:include(buildspecs/config_win.pri)

INCLUDEPATH += $${PWD}/plugins $${PWD}/libs $${PWD}/contrib $${PWD}/contrib/quazip
DEPENDPATH += $${PWD}/plugins $${PWD}/libs $${PWD}/contrib

LIBS *= -L$${BUILD_PLUGIN_PATH} -L$${BUILD_LIB_PATH}

# build mode
CONFIG *= qt warn_on thread x11 windows
CONFIG( debug, debug|release ) {
#  message( Building $${BINARY_TARGET} in DEBUG )
  DEFINES  *= DEBUG
} else {
#  message( Building $${BINARY_TARGET} in RELEASE )
  DEFINES  *= RELEASE
  # no warning from application
#  DEFINES  *= QT_NO_DEBUG_OUTPUT \
#              QT_NO_WARNING_OUTPUT \
#              QT_NO_DEBUG
}


TARGET   = $${BINARY_TARGET}
DESTDIR  = $${BUILD_BINARY_PATH}
