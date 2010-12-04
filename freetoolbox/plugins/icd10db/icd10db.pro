TEMPLATE = lib
TARGET = Icd10DB
PACKAGE_VERSION = 0.0.2

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

include(../../../plugins/fmf_plugins.pri)
include(icd10db_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    icd10databasecreator.cpp \
    icd10dbplugin.cpp \

HEADERS  += \
    icd10databasecreator.h \
    icd10dbplugin.h \


FORMS += \
    icd10databasewidget.ui


OTHER_FILES += \
    ../global_resources/sql/icd10.sql


OTHER_FILES += Icd10DB.pluginspec
