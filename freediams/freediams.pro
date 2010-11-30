TEMPLATE         = app
TARGET           = freediams
mac:TARGET       = $$quote(FreeDiams)
PACKAGE_VERSION  = 0.5.0
# Get the version number from command line in bash :
# cat this.pro | grep "PACKAGE_VERSION" -m 1 | cut -d = -s -f2 | tr -d ' '

# include general configuration
INSTALL_DRUGS = 1
FREEDIAMS = 1
include( ../config.pri )
!CONFIG(debug, release|debug):include( ../buildspecs/install.pri )

# include SDKs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
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

OTHER_FILES += freediams_win32_icon.rc
