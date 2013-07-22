TEMPLATE = lib
TARGET = Icd10DB
DEFINE += ICD10DB_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include(icd10db_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    icd10databasecreator.cpp \
    icd10dbplugin.cpp \
    icdgrouplinker.cpp \
    icd10step.cpp

HEADERS  += \
    icd10_exporter.h \
    icd10databasecreator.h \
    icd10dbplugin.h \
    icdgrouplinker.h \
    icd10step.h


FORMS += \
    icd10databasewidget.ui \
    icdgrouplinker.ui


OTHER_FILES += \
    ../global_resources/sql/icd10.sql


OTHER_FILES += Icd10DB.pluginspec
