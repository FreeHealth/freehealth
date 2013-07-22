TEMPLATE     = app
TARGET       = medintuxtoolkittester

include( ../../config.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri)
include( $${PACKAGE_LIBS_SOURCES}/sdk_medintuxtoolkit.pri)

DEFINES *= FMF_CORE_BUILD

# DESTDIR = $${PWD}/../bin
DEPENDPATH = $${PACKAGE_LIBS}/toolkit

PRE_TARGETDEPS *= ../../libs/toolkit

# define some path in order to access GLOBALRESOURCES
FMF_GLOBAL_RESOURCES = $${PWD}/../../../global_resources
mac:FMF_BIN_PATH = $${PWD}
else:unix|win32:FMF_BIN_PATH = $${PWD}
DEFINES += PACKAGE_VERSION \
    "FMF_BIN_PATH=\"\\\"$${FMF_BIN_PATH}\\\"\"" \
    "FMF_GLOBAL_RESOURCES=\"\\\"$$:FMF_GLOBAL_RESOURCES\\\"\""

# define Qt
QT *= sql network xml

# Compile
SOURCES += main.cpp
