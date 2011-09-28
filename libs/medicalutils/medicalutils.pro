# toolkit library project file
TEMPLATE        = lib
TARGET          = MedicalUtils

QT *= xml

DEFINES *= MEDICALUTILS_LIBRARY

# include config file
include( ../libsworkbench.pri )
include(../translationutils.pri)

include(aggir/aggir.pri)

HEADERS += global.h \
           medical_exporter.h \
    ebmdata.h \
    ebmmodel.h

SOURCES += global.cpp \
    ebmdata.cpp \
    ebmmodel.cpp


