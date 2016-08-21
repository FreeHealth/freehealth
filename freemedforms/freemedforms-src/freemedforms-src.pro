# first definitions
TEMPLATE = app
TARGET = $$quote(freehealth)

# Prepare installation
FMF_BUILD = 1
INSTALL_DRUGS = 1
INSTALL_ICD_DATABASE = 1
INSTALL_ACCOUNT_FILES = 1
INSTALL_ZIPCODES = 1
INSTALL_PROFILES_FILES = 1
INSTALL_EDRC_FILES = 1

# include general configuration
include(../../buildspecs/config.pri)
include($${SOURCES_BUILDSPECS_PATH}/install.pri)

# include libs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
include( $${SOURCES_LIBS_PATH}/translationutils.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
include( $${SOURCES_LIBS_PATH}/rpath.pri )

# define CORE BUILD for fmf exporter
DEFINES *= FMF_CORE_BUILD

# Qt definitions
QT *= xml \
    network \
    sql

macx {
  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/ehr.icns
  QMAKE_INFO_PLIST=Info.plist
}
win32:RC_FILE=ehr_win32_icon.rc

# Compile project
SOURCES += main.cpp

OTHER_FILES += Info.plist ehr_win32_icon.rc
