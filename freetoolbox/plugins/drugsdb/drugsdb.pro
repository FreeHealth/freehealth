TEMPLATE = lib
TARGET = DrugsDB

include(../../../plugins/fmf_plugins.pri)
include(drugsdb_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

HEADERS  += \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.h \
    tools.h \
    atcmodel.h \
    atcpage.h \
    drugsdbplugin.h \
    searchatcindatabasedialog.h \
    drug.h \
    drugdatabasedescription.h \
    routesmodel.h \
    drugsdbcore.h \
    idrugdatabasestep.h \
    idrugdatabasestepwidget.h \
    countries/frenchdrugsdatabasecreator.h \
    countries/fdadrugsdatabasecreator.h \
    countries/canadiandrugsdatabase.h \
    countries/belgishdrugsdatabase.h \
    countries/southafricandrugsdatabase.h \
#    countries/portuguesedrugsdatabase.h \
    countries/moleculelinkermodel.h \
    countries/moleculelinkerwidget.h \
    countries/moleculelinkdata.h \


SOURCES += \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.cpp \
    tools.cpp \
    atcmodel.cpp \
    atcpage.cpp \
    drugsdbplugin.cpp \
    searchatcindatabasedialog.cpp \
    drug.cpp \
    drugdatabasedescription.cpp \
    routesmodel.cpp \
    drugsdbcore.cpp \
    idrugdatabasestep.cpp \
    idrugdatabasestepwidget.cpp \
    countries/frenchdrugsdatabasecreator.cpp \
    countries/fdadrugsdatabasecreator.cpp \
    countries/canadiandrugsdatabase.cpp \
    countries/belgishdrugsdatabase.cpp \
    countries/southafricandrugsdatabase.cpp \
#    countries/portuguesedrugsdatabase.cpp \
    countries/moleculelinkermodel.cpp \
    countries/moleculelinkerwidget.cpp \
    countries/moleculelinkdata.cpp \


FORMS += \
#    frenchdrugsdatabasewidget.ui \
#    canadiandrugsdatabasewidget.ui \
#    fdadrugsdatabasewidget.ui \
#    southafricandrugsdatabase.ui \
#    belgishdrugsdatabase.ui \
#    portuguesedrugsdatabase.ui \
    countries/moleculelinkerwidget.ui \
    atcpage.ui \
    searchatcindatabasedialog.ui \
    idrugdatabasestepwidget.ui

OTHER_FILES += \
    ../global_resources/sql/drugs_schema.sql \
    ../global_resources/sql/moleculetoatclinker.xml \
    ../global_resources/sql/ia/atc_utf8.csv \
    ../global_resources/sql/drugdb/routes.txt \
    ../global_resources/sql/drugdb/be/description.xml \
    ../global_resources/sql/drugdb/be/licenceterms.txt \
    ../global_resources/sql/drugdb/fr/description.xml \
    ../global_resources/sql/drugdb/fr/licenceterms.txt \
    ../global_resources/sql/drugdb/za/description.xml \
    ../global_resources/sql/drugdb/za/licenceterms.txt \
#    ../global_resources/sql/drugdb/ca/canadian_db_creator.sql \
    ../global_resources/sql/drugdb/ca/description.xml \
    ../global_resources/sql/drugdb/ca/licenceterms.txt \
#    ../global_resources/sql/drugdb/ca/canadian_db_imports.sql \
#    ../global_resources/sql/drugdb/ca/canadian_db_processing.sql \
#    ../global_resources/sql/drugdb/ca/canadian_db_tables.sql \
    ../global_resources/sql/drugdb/us/description.xml \
    ../global_resources/sql/drugdb/us/licenceterms.txt \
    ../global_resources/sql/drugdb/za/description.xml \
    ../global_resources/sql/drugdb/za/licenceterms.txt \
    ../global_resources/sql/drugdb/za/za_uids.csv \
    ../global_resources/sql/drugdb/pt/description.xml \
    ../global_resources/sql/drugdb/pt/licenceterms.txt \
    ../global_resources/sql/drugdb/pt/pt_uids.csv \

# Interaction code
HEADERS += \
    ddi/drugdruginteraction.h \
    ddi/drugdruginteractionmodel.h \
    ddi/drugdruginteractioncore.h \
    ddi/interactioneditorpage.h \
    ddi/formalizeditemmodel.h \
    ddi/druginteractor.h \
    ddi/interactoreditorpage.h \
    ddi/afssapsintegrator.h
#    ddi/cytochromep450interactionspage.h \

SOURCES += \
    ddi/drugdruginteraction.cpp \
    ddi/drugdruginteractionmodel.cpp \
    ddi/drugdruginteractioncore.cpp \
    ddi/interactioneditorpage.cpp \
    ddi/formalizeditemmodel.cpp \
    ddi/druginteractor.cpp \
    ddi/interactoreditorpage.cpp \
    ddi/afssapsintegrator.cpp
#    ddi/cytochromep450interactionspage.cpp \

FORMS += \
    ddi/afssapslinkerwidget.ui \
    ddi/afssapstreewidget.ui \
    ddi/interactioneditorwidget.ui \
    ddi/afssapsintegratorwidget.ui \
    ddi/interactoreditorwidget.ui
#    ddi/cytochromep450interactionspage.ui \

OTHER_FILES += \
    ../global_resources/sql/drugs_schema.sql \
    ../global_resources/sql/iam_schema.sql \
    ../global_resources/sql/ia/atc_utf8.csv \
    ../global_resources/sql/ia/thesaurus_interactions.xml \
    ../global_resources/sql/ia/afssaps_links.xml \
    ../global_resources/sql/ia/afssaps_classtree.xml \
    ../global_resources/sql/drugdb/newdbschema.sql \
    ../global_resources/sql/drugdb/dbschema060_queries.sql

# Drugs infos (BIAM) + PIMs
HEADERS  += \
    biamextractor.h \
    pim/pimintegrator.h \
    pim/pimdatabasecreator.h \
    pim/pim_constants.h \
    preg/pregnancyclassification.h

SOURCES += \
    biamextractor.cpp \
    pim/pimintegrator.cpp \
    pim/pimdatabasecreator.cpp \
    preg/pregnancyclassification.cpp

FORMS += \
    biamextractor.ui \
    pim/pimintegrator.ui \
    pim/pimdatabasecreator.ui \
    preg/pregnancyclassification.ui

OTHER_FILES += \
    ../global_resources/sql/druginfodb/druginfos_schema.sql \
    ../global_resources/sql/ia/atc_utf8.csv \
    ../global_resources/sql/ia/afssaps_links.xml \
    ../global_resources/sql/ia/afssaps_classtree.xml \
    ../global_resources/sql/pims/icdlinks.xml \
    ../global_resources/sql/pims/pims.xml \
    ../global_resources/sql/druginfodb/biam2/biam_importer_schema.sql

OTHER_FILES += DrugsDB.pluginspec


