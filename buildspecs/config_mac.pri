macx{
    QMAKE_MACOSX_DEPLOYMENT_TARGET=10.11
    # define rpath
    CONFIG(debug_without_install) {
        RPATH_LIBS_BIN   = ../../../plugins/
    } else {
        RPATH_LIBS_BIN   = ../plugins/
    }
}
