TEMPLATE         = app
TARGET           = freetoolbox
mac:TARGET       = $$quote(FreeToolBox)

# Get the version number from command line in bash :
# cat this.pro | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '

# include general configuration
INSTALL_DRUGS = 0
INSTALL_ICD_DATABASE = 0
INSTALL_ACCOUNT_FILES = 0
INSTALL_ZIPCODES = 0
INSTALL_PROFILES_FILES = 0
include(../../buildspecs/config.pri)
include($${SOURCES_BUILDSPECS_PATH}/install.pri)

# include SDKs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
include( $${SOURCES_LIBS_PATH}/translationutils.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
include( $${SOURCES_LIBS_PATH}/rpath.pri )

# include Doc
# !CONFIG(crosscompil) {
#include( ../doc/manual-di.pri)
#PRE_TARGETDEPS += en_html_docs fr_html_docs
# }

QT *= sql \
    network \
    xml

DEFINES *= FREETOOLBOX

macx {
  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freetoolbox.icns
  QMAKE_INFO_PLIST=Info.plist
}
win32:RC_FILE=freetoolbox_win32_icon.rc

SOURCES += \
    main.cpp

OTHER_FILES += freetoolbox_win32_icon.rc
