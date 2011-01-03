TEMPLATE         = app
TARGET           = freepad
mac:TARGET       = $$quote(FreePad)
PACKAGE_VERSION  = 0.1.0

# include general configuration
FREEPAD = 1

include( ../config.pri )
!CONFIG(debug, release|debug):include(../buildspecs/install.pri)

message($${TARGET}  $${DESTDIR})

# include SDKs
include($${SOURCES_LIBS_PATH}/extensionsystem.pri)
include($${SOURCES_LIBS_PATH}/utils.pri)
include($${SOURCES_LIBS_PATH}/rpath.pri)

QT *= sql \
    network \
    xml

DEFINES *= FREEPAD

macx {
#  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freepad.icns
#  QMAKE_INFO_PLIST=Info.plist
}
#win32:RC_FILE=freepad_win32_icon.rc

# sources
SOURCES += \
    main.cpp

OTHER_FILES += freepad_win32_icon.rc Info.plist
