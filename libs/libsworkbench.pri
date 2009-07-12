# include general workbench
include(../config.pri)

win32 {
    DLLDESTDIR = $${PACKAGE_DESTDIR}
}

DESTDIR  = $${PACKAGE_LIBS_BIN}
include(../rpath.pri)

# TARGET = $$qtLibraryTarget($$TARGET)
