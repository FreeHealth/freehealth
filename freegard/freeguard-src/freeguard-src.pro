TEMPLATE         = app
TARGET           = freeguard
mac:TARGET       = $$quote(FreeGuard)

BUILD_PATH_POSTFIXE = FreeGuard

# include general configuration
INSTALL_PROFILES_FILES = 0
INSTALL_DRUGS = 0
INSTALL_ICD_DATABASE = 0
INSTALL_ACCOUNT_FILES = 0
INSTALL_ZIPCODES = 0
INSTALL_EDRC_FILES = 0

include(../../buildspecs/config.pri)
include($${SOURCES_BUILDSPECS_PATH}/install.pri)

# include SDKs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
include( $${SOURCES_LIBS_PATH}/translationutils.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
include( $${SOURCES_LIBS_PATH}/rpath.pri )

QT *= sql \
    network \
    xml

DEFINES *= FREEGUARD

macx {
  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freeguard.icns
  QMAKE_INFO_PLIST=Info.plist
}
win32:RC_FILE=freeguard_win32_icon.rc

# sources
SOURCES += \
    main.cpp \

OTHER_FILES += freeguard_win32_icon.rc Info.plist
