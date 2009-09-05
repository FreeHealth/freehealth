# include general workbench
include(../config.pri)

win32 {
    DLLDESTDIR = $${BUID_BINARY_PATH}
}

CONFIG *= dll plugin

DESTDIR  = $${BUILD_LIB_PATH}
include(rpath.pri)

# TARGET = $$qtLibraryTarget($$TARGET)
