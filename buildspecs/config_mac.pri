macx{
    # Qt 4.7 does not support ppc by default
    #CONFIG *= x86 x86-64
    CONFIG  -= ppc
    QMAKE_MACOSX_DEPLOYMENT_TARGET=10.11

    # define rpath
    CONFIG(debug_without_install) {
        RPATH_LIBS_BIN   = ../../../plugins/
    } else {
        RPATH_LIBS_BIN   = ../plugins/
    }

    # fix linking visibility problem on mac: https://bugreports.qt-project.org/browse/QTBUG-3995
    QMAKE_CFLAGS_RELEASE += -fvisibility=hidden
    QMAKE_CXXFLAGS_RELEASE += -fvisibility=hidden -fvisibility-inlines-hidden

    QMAKE_CFLAGS_DEBUG += -fvisibility=hidden
    QMAKE_CXXFLAGS_DEBUG += -fvisibility=hidden -fvisibility-inlines-hidden
}
