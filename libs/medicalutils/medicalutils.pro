# toolkit library project file
TEMPLATE        = lib
TARGET          = MedicalUtils
PACKAGE_VERSION = 0.0.2

DEFINES *= MEDICALUTILS_LIBRARY

# include config file
include( ../libsworkbench.pri )
include(../translationutils.pri)

#CONFIG *= plugins

include(aggir/aggir.pri)

HEADERS += global.h \
           medical_exporter.h

SOURCES += global.cpp
