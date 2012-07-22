# include general workbench
include(../buildspecs/config.pri)

win32 {
    DLLDESTDIR = $${BUILD_BINARY_PATH}
}

include(rpath.pri)

CONFIG *= shared dll

DESTDIR  = $${BUILD_LIB_PATH}


# TARGET = $$qtLibraryName($$TARGET)

#!macx {
#    win32 {
#        target.path = /bin
#    } else {
#        target.path = /$$IDE_LIBRARY_BASENAME/qtcreator
#    }
#    INSTALLS += target
#}
