# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

# include general workbench
include(../buildspecs/config.pri)
win32 {
    DLLDESTDIR = $${BUILD_BINARY_PATH}
}

include(rpath.pri)
CONFIG *= shared dll
DESTDIR  = $${BUILD_LIB_PATH}

# manage pre tag translations
PRE_TRANSLATION  = lib
