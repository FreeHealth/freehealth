TEMPLATE = lib
TARGET = DrugInfos
PACKAGE_VERSION = 0.0.2

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/fmf_plugins.pri)
include(druginfos_dependencies.pri)

QT *= sql network xml script

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    druginfosplugin.cpp \
    biamextractor.cpp \
    pimintegrator.cpp \
    pimdatabasecreator.cpp \
    pregnancyclassification.cpp


HEADERS  += \
    druginfosplugin.h \
    biamextractor.h \
    pimintegrator.h \
    pimdatabasecreator.h \
    pim_constants.h \
    pregnancyclassification.h


FORMS += \
    biamextractor.ui \
    pimintegrator.ui \
    pimdatabasecreator.ui


OTHER_FILES += \
    ../global_resources/sql/druginfodb/druginfos_schema.sql \
    ../global_resources/sql/ia/atc_utf8.csv \
    ../global_resources/sql/ia/afssaps_links.xml \
    ../global_resources/sql/ia/afssaps_classtree.xml \
    ../global_resources/sql/pims/icdlinks.xml \
    ../global_resources/sql/pims/pims.xml \
    ../global_resources/sql/druginfodb/biam2/biam_importer_schema.sql


OTHER_FILES += DrugInfos.pluginspec


