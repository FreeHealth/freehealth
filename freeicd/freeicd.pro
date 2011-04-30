TEMPLATE         = app
TARGET           = freeicd
mac:TARGET       = $$quote(FreeICD)
PACKAGE_VERSION  = 0.1.0

# include general configuration
INSTALL_ICD_DATABASE = 1
FREEICD = 1

include( ../config.pri )
!CONFIG(debug, release|debug):include(../buildspecs/install.pri)

# include SDKs
include($${SOURCES_LIBS_PATH}/extensionsystem.pri)
include($${SOURCES_LIBS_PATH}/utils.pri)
include($${SOURCES_LIBS_PATH}/rpath.pri)

QT *= sql \
    network \
    xml

DEFINES *= FREEICD

macx {
  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freeicd.icns
  QMAKE_INFO_PLIST=Info.plist
}
win32:RC_FILE=freeicd_win32_icon.rc

# sources
SOURCES += \
    main.cpp

OTHER_FILES += freeicd_win32_icon.rc Info.plist
