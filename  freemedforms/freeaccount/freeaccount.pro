TEMPLATE         = app
TARGET           = freeaccount
mac:TARGET       = $$quote(FreeAccount)
PACKAGE_VERSION  = 0.1.0

# include general configuration
INSTALL_DRUGS = 0
include( ../config.pri )
!CONFIG(debug, release|debug):include( ../buildspecs/install.pri )

# include SDKs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
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

#macx:ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freecount.icns
#win32:RC_FILE=$${SOURCES_GLOBAL_RESOURCES}/textfiles/freecount_win32_icon.rc

# sources
SOURCES += \
    main.cpp \


