# include general workbench
include(../config.pri)

win32 {
    DLLDESTDIR = $${PACKAGE_DESTDIR}
}

#DESTDIR = $$IDE_LIBRARY_PATH
DESTDIR  = $${PACKAGE_LIBS_BIN}
include(../rpath.pri)

# TARGET = $$qtLibraryTarget($$TARGET)
