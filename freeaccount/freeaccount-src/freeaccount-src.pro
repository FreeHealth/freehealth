TEMPLATE         = app
TARGET           = freeaccount
mac:TARGET       = $$quote(FreeAccount)

BUILD_PATH_POSTFIXE = FreeAccount

# include general configuration
INSTALL_DRUGS = 0
INSTALL_ACCOUNT_FILES = 1
INSTALL_PROFILES_FILES = 1
INSTALL_ZIPCODES = 1

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

DEFINES *= FREEACCOUNT

macx {
  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freeaccount.icns
  #QMAKE_INFO_PLIST=Info.plist
}
win32:RC_FILE=freeaccount_win32_icon.rc

# sources
SOURCES += \
    main.cpp \

OTHER_FILES += freeaccount_win32_icon.rc Info.plist
