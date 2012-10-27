TEMPLATE = lib
TARGET = DrugsDB

include(../../../plugins/fmf_plugins.pri)
include(drugsdb_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.cpp \
    tools.cpp \
    frenchdrugsdatabasecreator.cpp \
#    canadiandrugsdatabase.cpp \
#    fdadrugsdatabasecreator.cpp \
#    southafricandrugsdatabase.cpp \
#    belgishdrugsdatabase.cpp \
#    portuguesedrugsdatabase.cpp \
    moleculelinkermodel.cpp \
    moleculelinkerwidget.cpp \
    atcmodel.cpp \
    atcpage.cpp \
    drugsdbplugin.cpp \
    searchatcindatabasedialog.cpp \
    drug.cpp \
    drugdatabasedescription.cpp \
    routesmodel.cpp \
    drugsdbcore.cpp \
    idrugdatabasestep.cpp \
    moleculelinkdata.cpp \
    idrugdatabasestepwidget.cpp

HEADERS  += \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.h \
    tools.h \
    frenchdrugsdatabasecreator.h \
#    canadiandrugsdatabase.h \
#    fdadrugsdatabasecreator.h \
#    southafricandrugsdatabase.h \
#    belgishdrugsdatabase.h \
#    portuguesedrugsdatabase.h \
    moleculelinkermodel.h \
    moleculelinkerwidget.h \
    atcmodel.h \
    atcpage.h \
    drugsdbplugin.h \
    searchatcindatabasedialog.h \
    drug.h \
    drugdatabasedescription.h \
    routesmodel.h \
    drugsdbcore.h \
    idrugdatabasestep.h \
    moleculelinkdata.h \
    idrugdatabasestepwidget.h

FORMS += \
#    frenchdrugsdatabasewidget.ui \
#    canadiandrugsdatabasewidget.ui \
#    fdadrugsdatabasewidget.ui \
#    southafricandrugsdatabase.ui \
#    belgishdrugsdatabase.ui \
#    portuguesedrugsdatabase.ui \
    moleculelinkerwidget.ui \
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
    interactionstep.h \
    drugdruginteraction.h \
    drugdruginteractionmodel.h \
    drugdruginteractioncore.h \
    interactioneditorpage.h \
    formalizeditemmodel.h \
    druginteractor.h \
    interactoreditorpage.h \
    afssapsintegrator.h \
    interactionsdatabasepage.h

#    cytochromep450interactionspage.h \

SOURCES += \
    interactionstep.cpp \
    drugdruginteraction.cpp \
    drugdruginteractionmodel.cpp \
    drugdruginteractioncore.cpp \
    interactioneditorpage.cpp \
    formalizeditemmodel.cpp \
    druginteractor.cpp \
    interactoreditorpage.cpp \
    afssapsintegrator.cpp \
    interactionsdatabasepage.cpp
#    cytochromep450interactionspage.cpp \

FORMS += \
    afssapslinkerwidget.ui \
    afssapstreewidget.ui \
    interactioneditorwidget.ui \
    interactiondatabasecreator.ui \
    interactiondatabasechecker.ui \
    afssapsintegratorwidget.ui \
    interactoreditorwidget.ui
#    cytochromep450interactionspage.ui \

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
SOURCES += \
    biamextractor.cpp \
    pimintegrator.cpp \
    pimdatabasecreator.cpp \
    pregnancyclassification.cpp

HEADERS  += \
    biamextractor.h \
    pimintegrator.h \
    pimdatabasecreator.h \
    pim_constants.h \
    pregnancyclassification.h

FORMS += \
    biamextractor.ui \
    pimintegrator.ui \
    pimdatabasecreator.ui \
    pregnancyclassification.ui

OTHER_FILES += \
    ../global_resources/sql/druginfodb/druginfos_schema.sql \
    ../global_resources/sql/ia/atc_utf8.csv \
    ../global_resources/sql/ia/afssaps_links.xml \
    ../global_resources/sql/ia/afssaps_classtree.xml \
    ../global_resources/sql/pims/icdlinks.xml \
    ../global_resources/sql/pims/pims.xml \
    ../global_resources/sql/druginfodb/biam2/biam_importer_schema.sql

OTHER_FILES += DrugsDB.pluginspec


