TEMPLATE = lib
TARGET = DrugInfos
PACKAGE_VERSION = 0.0.2

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/fmf_plugins.pri)
include(druginfos_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    druginfosplugin.cpp \
    biamextractor.cpp


HEADERS  += \
    druginfosplugin.h \
    biamextractor.h


FORMS += \
    biamextractor.ui


OTHER_FILES += \
    ../global_resources/sql/druginfos_schema.sql \
    ../global_resources/sql/biam_importer_schema.sql


OTHER_FILES += DrugInfos.pluginspec
