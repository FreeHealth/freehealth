# toolkit library project file
TEMPLATE = lib
TARGET = medicaltoolkit
PACKAGE_VERSION = 0.0.1
!win32:VERSION = $${PACKAGE_VERSION}

# include config file
include( ../libsworkbench.pri )
include( ../sdk_toolkit.pri )

# include sdk
INCLUDEPATH *= $${PWD}/sdk

# define Qt
QT *= sql \
    network \
    xml
CONFIG *= dll

# make library exportable
DEFINES *= MEDICAL_TOOLKIT_CORE_BUILD

# translators
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/medicaltoolkit_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/medicaltoolkit_de.ts
HEADERS += objects/tkMedicalGlobal.h \
    tkMedicalExporter.h
SOURCES += objects/tkMedicalGlobal.cpp
