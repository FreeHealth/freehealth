TEMPLATE         = app
TARGET           = freediams
mac:TARGET       = $$quote(FreeDiams)
PACKAGE_VERSION  = 0.1.0

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
PRE_TARGETDEPS += html_docs
# }

# define CORE BUILD for fmf exporter
DEFINES *= CORE_BUILD

QT *= sql \
    network \
    xml
DEFINES *= FREEDIAMS

# sources
SOURCES += \
    main.cpp \
