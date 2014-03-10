TEMPLATE        = lib
TARGET          = DrugsDb

DEFINES += DRUGSDB_LIBRARY

include(../../../plugins/fmf_plugins.pri)
include($${PWD}/drugsdbplugin_dependencies.pri)

INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS += \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.h \
    constants.h \
    tools.h \
    drug.h \
    drugsdbplugin.h \
    imode/drugsdbmode.h \
    imode/drugsdbmodewidget.h \
    drugdatabasedescription.h \
    idrugdatabase.h \
#    countries/be/belgishdrugsdatabase.h \
#    countries/ca/canadiandrugsdatabase.h \
    countries/fr/frenchdrugsdatabasecreator.h \
#    countries/pt/portuguesedrugsdatabase.h \
    countries/us/fdadrugsdatabase.h \
    countries/za/southafricandrugsdatabase.h \
    drugdatabasepopulator.h \


SOURCES += \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.cpp \
    drugsdbplugin.cpp \
    imode/drugsdbmode.cpp \
    imode/drugsdbmodewidget.cpp \
    constants.cpp \
    tools.cpp \
    drug.cpp \
    drugdatabasedescription.cpp \
    idrugdatabase.cpp \
#    countries/be/belgishdrugsdatabase.cpp \
#    countries/ca/canadiandrugsdatabase.cpp \
    countries/fr/frenchdrugsdatabasecreator.cpp \
#    countries/pt/portuguesedrugsdatabase.cpp \
    countries/us/fdadrugsdatabase.cpp \
    countries/za/southafricandrugsdatabase.cpp \
    drugdatabasepopulator.cpp \


FORMS += \
    imode/drugsdbmodewidget.ui \
#    countries/za/southafricandrugsdatabase.ui \
#    countries/us/fdadrugsdatabasewidget.ui \
#    countries/pt/portuguesedrugsdatabase.ui \
#    countries/be/belgishdrugsdatabase.ui \
#    countries/ca/canadiandrugsdatabasewidget.ui \


OTHER_FILES += DrugsDb.pluginspec

#equals(TEST, 1) {
#    SOURCES += \
#        tests/test_.cpp \
#        tests/test_.cpp \
#        tests/test_init_clean.cpp
#}

# include translations
TRANSLATION_NAME = drugsdb
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
