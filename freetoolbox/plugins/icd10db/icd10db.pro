TEMPLATE = lib
TARGET = Icd10DB

include(../../../plugins/fmf_plugins.pri)
include(icd10db_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    icd10databasecreator.cpp \
    icd10dbplugin.cpp \
    icdgrouplinker.cpp

HEADERS  += \
    icd10databasecreator.h \
    icd10dbplugin.h \
    icdgrouplinker.h


FORMS += \
    icd10databasewidget.ui \
    icdgrouplinker.ui


OTHER_FILES += \
    ../global_resources/sql/icd10.sql


OTHER_FILES += Icd10DB.pluginspec
