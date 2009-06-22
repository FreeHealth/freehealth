# toolkit library project file
TEMPLATE         = lib
TARGET           = medintuxtoolkit
PACKAGE_VERSION  = 0.0.1
!win32:VERSION   = 0.0.1

# include config file
include( ../libsworkbench.pri )
include( ../sdk_toolkit.pri )

# include sdk
INCLUDEPATH *= $${PWD}/sdk

# define Qt
QT *= sql
﻿CONFIG *= dll

# make library exportable
DEFINES *= MEDINTUXTOOLKIT_CORE_BUILD

PRE_TARGETDEPS *= $${PACKAGE_LIBS_SOURCES}/toolkit

CONFIG( debug, debug|release ) {
  win32:LIBS *= -ltoolkit_d
  else:LIBS *= -ltoolkit_debug
} else {
  LIBS *= -ltoolkit
}

include( ../rpath.pri )

# Core
HEADERS += objects/tkMedintuxConfiguration.h \
    tkMedintuxExporter.h

# protected files
exists( $${PROTECTED_PATH} ):SOURCES += $${PROTECTED_PATH}/medintuxtoolkit/objects/tkMedintuxConfiguration_Pro.cpp
else:SOURCES += objects/tkMedintuxConfiguration.cpp

# translators
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/medintuxtoolkit_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/medintuxtoolkit_de.ts
