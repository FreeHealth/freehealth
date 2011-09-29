# first definitions
TEMPLATE = app
macx:win32:TARGET = $$quote(FreeMedForms)
else:TARGET = $$quote(freemedforms)
PACKAGE_VERSION = 0.5.9

# Prepare installation
FMF_BUILD = 1
INSTALL_DRUGS = 1
INSTALL_ICD_DATABASE = 1
INSTALL_ACCOUNT_FILES = 1
INSTALL_ZIPCODES = 1

# include general configuration
include( ../config.pri )
!CONFIG(debug, release|debug):include( ../buildspecs/install.pri )

# include libs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
include( $${SOURCES_LIBS_PATH}/rpath.pri )

# define CORE BUILD for fmf exporter
DEFINES *= FMF_CORE_BUILD

# Qt definitions
QT *= xml \
    network \
    sql

macx {
  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freemedforms.icns
  QMAKE_INFO_PLIST=Info.plist
}
win32:RC_FILE=freemedforms_win32_icon.rc

# Compile project
SOURCES += main.cpp
