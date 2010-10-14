TEMPLATE         = app
TARGET           = freeicd
mac:TARGET       = $$quote(FreeICD)
PACKAGE_VERSION  = 0.4.0

# include general configuration
include( ../config.pri )
!CONFIG(debug, release|debug):include(../buildspecs/install.pri)

# include SDKs
include($${SOURCES_LIBS_PATH}/extensionsystem.pri)
include($${SOURCES_LIBS_PATH}/utils.pri)
include($${SOURCES_LIBS_PATH}/rpath.pri)

# include Doc
# !CONFIG(crosscompil) {
#include( ../doc/manual-di.pri)
#PRE_TARGETDEPS += en_html_docs fr_html_docs
# }

QT *= sql \
    network \
    xml

DEFINES *= FREEICD

#macx {
#  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freeicd.icns
#  QMAKE_INFO_PLIST=Info.plist
#}
#win32:RC_FILE=freeicd_win32_icon.rc

# sources
SOURCES += \
    main.cpp

OTHER_FILES += freeicd_win32_icon.rc
