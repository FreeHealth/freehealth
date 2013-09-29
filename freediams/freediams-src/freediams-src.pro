TEMPLATE         = app
TARGET           = freediams
mac:TARGET       = $$quote(FreeDiams)

# include general configuration
FREEDIAMS = 1
INSTALL_DRUGS = 1
INSTALL_ICD_DATABASE = 1
INSTALL_ACCOUNT_FILES = 0
INSTALL_ZIPCODES = 0
INSTALL_PROFILES_FILES = 0
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

DEFINES *= FREEDIAMS

macx {
  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freediams.icns
  QMAKE_INFO_PLIST=Info.plist
}
win32:RC_FILE=freediams_win32_icon.rc

# sources
SOURCES += \
    main.cpp \

OTHER_FILES += freediams_win32_icon.rc \
               ../../update-di.txt \
               ../../global_resources/textfiles/freediams.desktop \
               #global_resources/textfiles/freediams_win32_icon.rc \
               ../../global_resources/textfiles/dataintest.xml \
               ../../global_resources/testexchangeout.xml \
               Info.plist
