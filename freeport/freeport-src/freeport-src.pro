TEMPLATE         = app
TARGET           = freeport
mac:TARGET       = $$quote(FreePort)

# include general configuration
FREEPORT = 1

include(../../buildspecs/config.pri)
include($${SOURCES_BUILDSPECS_PATH}/install.pri)

message($${TARGET}  $${DESTDIR})

# include SDKs
include($${SOURCES_LIBS_PATH}/extensionsystem.pri)
include( $${SOURCES_LIBS_PATH}/translationutils.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
include($${SOURCES_LIBS_PATH}/rpath.pri)

QT *= sql \
    network \
    xml

DEFINES *= FREEPAD

#macx {
#  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freeport.icns
#  QMAKE_INFO_PLIST=Info.plist
#}
#win32:RC_FILE=freeport_win32_icon.rc

# sources
SOURCES += \
    main.cpp

OTHER_FILES += freeport_win32_icon.rc Info.plist
