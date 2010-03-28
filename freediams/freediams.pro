TEMPLATE         = app
TARGET           = freediams
mac:TARGET       = $$quote(FreeDiams)
PACKAGE_VERSION  = 0.3.9

# include general configuration
INSTALL_DRUGS = 1
include( ../config.pri )
!CONFIG(debug, release|debug):include( ../buildspecs/install.pri )

# include SDKs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
include( $${SOURCES_LIBS_PATH}/rpath.pri )

# include Doc
# !CONFIG(crosscompil) {
include( ../doc/manual-di.pri)
PRE_TARGETDEPS += en_html_docs fr_html_docs
# }

QT *= sql \
    network \
    xml

DEFINES *= FREEDIAMS

macx {
  ICON=$${SOURCES_GLOBAL_RESOURCES}/pixmap/svg/freediams.icns
  !CONFIG(debug, release|debug):QMAKE_INFO_PLIST=Info.plist
}
win32:RC_FILE=freediams_win32_icon.rc

# sources
SOURCES += \
    main.cpp \

OTHER_FILES += freediams_win32_icon.rc
