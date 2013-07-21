TEMPLATE        = lib
TARGET          = MedicalUtils

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

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

# include translations
TRANSLATION_NAME = medical
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
