# prepare package name // TARGET __MUST BE__ defined BEFORE this file is included
isEmpty(TARGET) {
    error("config.pri: You must provide a TARGET")
}

# include the generic cofiguration file (define some paths)
SOURCES_ROOT_PATH        = $${PWD}
include(buildspecs/config_paths.pri)

# the library base name is used only during the install procedure to locate the path
# the application's librairies and plugins should be installed
# you can specify it when compiling = qmake -r -config release "LIBRARY_BASENAME=lib64" for 64bit compilation
# this value is only used with linux compilation
isEmpty(LIBRARY_BASENAME) {
    LIBRARY_BASENAME = lib
}
DEFINES *= LIBRARY_BASENAME=\\\"$$LIBRARY_BASENAME\\\"

macx:include(buildspecs/config_mac.pri)
else:unix:include(buildspecs/config_linux.pri)
else:win32:include(buildspecs/config_win.pri)

INCLUDEPATH += $${PWD}/plugins $${PWD}/libs

LIBS *= -L$${BUILD_PLUGIN_PATH} -L$${BUILD_LIB_PATH}

CONFIG( debug, debug|release ) {
  message( Building $${BINARY_TARGET} in DEBUG )
  DEFINES  *= DEBUG
} else {
  message( Building $${BINARY_TARGET} in RELEASE )
  DEFINES  *= RELEASE
  # no warning from application
  DEFINES  *= QT_NO_DEBUG_OUTPUT \
              QT_NO_WARNING_OUTPUT \
              QT_NO_DEBUG
}

DEFINES	*= "BINARY_NAME=\"\\\"$${BINARY_TARGET}\\\"\"" \
           "PACKAGE_VERSION=\"\\\"$${PACKAGE_VERSION}\\\"\""

TARGET   = $${BINARY_TARGET}
DESTDIR  = $${BUID_BINARY_PATH}
