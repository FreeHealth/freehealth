TEMPLATE = lib
TARGET = ZipCodes

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/fmf_plugins.pri)
include(zipcodes_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    zipcodesplugin.cpp \
    frenchzipcodespage.cpp \
    genericzipcodespage.cpp \
    genericzipcodeswidget.cpp \
    genericzipcodesstep.cpp


HEADERS  += \
    zipcodesplugin.h \
    frenchzipcodespage.h \
    genericzipcodespage.h \
    genericzipcodeswidget.h \
    genericzipcodesstep.h


FORMS += \
    frenchzipcodeswidget.ui \
    genericzipcodeswidget.ui


OTHER_FILES += \
    ../global_resources/sql/zipcodes/zipcodes.sql

OTHER_FILES += ZipCodes.pluginspec
