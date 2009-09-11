# toolkit library project file
TEMPLATE         = lib
TARGET           = Medintux
PACKAGE_VERSION  = 0.1.0
!win32:VERSION   = $${PACKAGE_VERSION}

# include config file
include( ../libsworkbench.pri )
include( ../medintuxutils_dependencies.pri )

# include sdk
INCLUDEPATH *= $${PWD}/sdk

# define Qt
QT *= sql
CONFIG *= dll

# make library exportable
DEFINES *= MEDINTUX_LIBRARY

# Core
HEADERS += configuration.h \
    medintux_exporter.h

# protected files
exists( $${SOURCES_PROTECTED_PATH} ):SOURCES += $${SOURCES_PROTECTED_PATH}/medintuxtoolkit/configuration_pro.cpp
else:SOURCES += configuration.cpp

# translators
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/medintuxtoolkit_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/medintuxtoolkit_de.ts
