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

# Manage Qt >= 5
greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets
