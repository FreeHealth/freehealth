# first definitions
TEMPLATE = app
TARGET = $$quote(FreeMedForms)
PACKAGE_VERSION = 0.0.9

# Prepare installation
FMF_BUILD = 1
INSTALL_DRUGS = 1

# include general configuration
include( ../config.pri )
!CONFIG(debug, release|debug):include( ../buildspecs/install.pri )

# include libs
include( $${SOURCES_LIBS_PATH}/extensionsystem.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri )
include( $${SOURCES_LIBS_PATH}/rpath.pri )

# include Doc
include( ../doc/manual-fmf.pri)
PRE_TARGETDEPS += html_docs

# define CORE BUILD for fmf exporter
DEFINES *= FMF_CORE_BUILD

# Qt definitions
QT *= xml \
    network \
    script \
    sql

# Compile project
SOURCES += main.cpp
