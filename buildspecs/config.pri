# prepare package name // TARGET __MUST BE__ defined BEFORE this file is included
isEmpty(TARGET) {
    error("config.pri: You must provide a TARGET")
}

# lowered appname must be specified
include(compiling.pri)
isEmpty(LOWERED_APPNAME) {
    error(Missing LOWERED_APPNAME in qmake command line. Eg: qmake -r freediams.pro LOWERED_APPNAME=freediams)
}

# Qt cache system
cache()

# auto-test for plugins
isEmpty(TEST):CONFIG(debug, debug|release) {
    !debug_and_release|build_pass {
        TEST = 1
    }
}
equals(TEST, 1) {
    QT += testlib
    DEFINES += WITH_TESTS
}

# check optional build features
include(optionalfeatures.pri)

# include the generic configuration file (define some paths)
SOURCES_ROOT_PATH        = $${PWD}/..
include(config_paths.pri)

exists(__nonfree__):include(__nonfree__/config_nonfree.pri)

macx:include(config_mac.pri)
else:linux*:include(config_linux.pri)
else:win32:include(config_win.pri)
include(githash.pri)

INCLUDEPATH += \
    $${SOURCES_PLUGINS_PATH} \
    $${SOURCES_LIBS_PATH} \
    $${SOURCES_CONTRIBS_PATH} \
    $${SOURCES_CONTRIBS_PATH}/quazip

DEPENDPATH += \
    $${SOURCES_PLUGINS_PATH} \
    $${SOURCES_LIBS_PATH} \
    $${SOURCES_CONTRIBS_PATH} \
    $${SOURCES_CONTRIBS_PATH}/quazip

CONFIG += depend_includepath

LIBS *= -L$${BUILD_PLUGIN_PATH} -L$${BUILD_LIB_PATH}

# build mode
CONFIG *= qt warn_on thread x11 windows

#!macx:DEFINES += QT_USE_FAST_OPERATOR_PLUS QT_USE_FAST_CONCATENATION
DEFINES += QT_USE_FAST_OPERATOR_PLUS QT_USE_FAST_CONCATENATION

CONFIG(debug, debug|release) {
#  message( Building $${BINARY_TARGET} in DEBUG )
  DEFINES  *= DEBUG
  CONFIG += console
  CONFIG(debug_without_install):DEFINES *= DEBUG_WITHOUT_INSTALL
} else {
#  message( Building $${BINARY_TARGET} in RELEASE )
  DEFINES  *= RELEASE
  # no warning from application
#  DEFINES  *= QT_NO_DEBUG_OUTPUT \
#              QT_NO_WARNING_OUTPUT \
#              QT_NO_DEBUG
}

contains(QT, core): QT += concurrent
contains(QT, gui): QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x040900

TARGET   = $${BINARY_TARGET}
DESTDIR  = $${BUILD_BINARY_PATH}
