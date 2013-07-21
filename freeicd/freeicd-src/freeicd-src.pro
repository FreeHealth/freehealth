TEMPLATE         = app
TARGET           = freeicd
mac:TARGET       = $$quote(FreeICD)

# include general configuration
FREEICD = 1
INSTALL_ICD_DATABASE = 1
INSTALL_DRUGS = 0
INSTALL_ACCOUNT_FILES = 0
INSTALL_ZIPCODES = 0
INSTALL_PROFILES_FILES = 0

include(../../buildspecs/config.pri)
!CONFIG(debug, release|debug):include($${SOURCES_BUILDSPECS_PATH}/install.pri)

# include SDKs
include($${SOURCES_LIBS_PATH}/extensionsystem.pri)
include( $${SOURCES_LIBS_PATH}/translationutils.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
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
