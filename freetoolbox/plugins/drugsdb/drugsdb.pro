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
    drugsdbplugin.h \
    searchatcindatabasedialog.h \
    drug.h \
    drugdatabasedescription.h \
    routesmodel.h \
    drugsdbcore.h \
    idrugdatabasestep.h \
    idrugdatabasestepwidget.h \
    \
    atc/atcmodel.h \
    atc/atcpage.h \
    \
    countries/frenchdrugsdatabasecreator.h \
    countries/fdadrugsdatabasecreator.h \
    countries/canadiandrugsdatabase.h \
    countries/belgishdrugsdatabase.h \
    countries/southafricandrugsdatabase.h \
#    countries/portuguesedrugsdatabase.h \
    countries/moleculelinkermodel.h \
    countries/moleculelinkerwidget.h \
    countries/moleculelinkdata.h \
    \
    ddi/drugdruginteraction.h \
    ddi/drugdruginteractionmodel.h \
    ddi/drugdruginteractioncore.h \
    ddi/interactioneditorpage.h \
    ddi/formalizeditemmodel.h \
    ddi/druginteractor.h \
    ddi/interactoreditorpage.h \
    ddi/afssapsintegrator.h \
    ddi/interactorcompleter.h \
#    ddi/cytochromep450interactionspage.h \
    \
    biamextractor.h \
    pim/pimintegrator.h \
    pim/pimdatabasecreator.h \
    pim/pim_constants.h \
    \
    preg/pregnancyclassification.h \
    ddi/interactorselectordialog.h \
    ddi/drugdruginteractiondatabasepopulator.h

SOURCES += \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.cpp \
    tools.cpp \
    drugsdbplugin.cpp \
    searchatcindatabasedialog.cpp \
    drug.cpp \
    drugdatabasedescription.cpp \
    routesmodel.cpp \
    drugsdbcore.cpp \
    idrugdatabasestep.cpp \
    idrugdatabasestepwidget.cpp \
    \
    atc/atcmodel.cpp \
    atc/atcpage.cpp \
    \
    countries/frenchdrugsdatabasecreator.cpp \
    countries/fdadrugsdatabasecreator.cpp \
    countries/canadiandrugsdatabase.cpp \
    countries/belgishdrugsdatabase.cpp \
    countries/southafricandrugsdatabase.cpp \
#    countries/portuguesedrugsdatabase.cpp \
    countries/moleculelinkermodel.cpp \
    countries/moleculelinkerwidget.cpp \
    countries/moleculelinkdata.cpp \
    \
    ddi/drugdruginteraction.cpp \
    ddi/drugdruginteractionmodel.cpp \
    ddi/drugdruginteractioncore.cpp \
    ddi/interactioneditorpage.cpp \
    ddi/formalizeditemmodel.cpp \
    ddi/druginteractor.cpp \
    ddi/interactoreditorpage.cpp \
    ddi/afssapsintegrator.cpp \
    ddi/interactorcompleter.cpp \
#    ddi/cytochromep450interactionspage.cpp \
    \
    biamextractor.cpp \
    pim/pimintegrator.cpp \
    pim/pimdatabasecreator.cpp \
    preg/pregnancyclassification.cpp \
    ddi/interactorselectordialog.cpp \
    ddi/drugdruginteractiondatabasepopulator.cpp


FORMS += \
    countries/moleculelinkerwidget.ui \
    searchatcindatabasedialog.ui \
    idrugdatabasestepwidget.ui \
    \
    atc/atcpage.ui \
    \
    ddi/afssapslinkerwidget.ui \
    ddi/afssapstreewidget.ui \
    ddi/interactioneditorwidget.ui \
    ddi/afssapsintegratorwidget.ui \
    ddi/interactoreditorwidget.ui \
#    ddi/cytochromep450interactionspage.ui \
    \
    biamextractor.ui \
    pim/pimintegrator.ui \
    pim/pimdatabasecreator.ui \
    preg/pregnancyclassification.ui \
    ddi/interactorselectorwidget.ui


OTHER_FILES += \
    ../global_resources/sql/drugdb/routes.txt \
    ../global_resources/sql/drugdb/be/description.xml \
    ../global_resources/sql/drugdb/be/licenceterms.txt \
    ../global_resources/sql/drugdb/fr/description.xml \
    ../global_resources/sql/drugdb/fr/licenceterms.txt \
    ../global_resources/sql/drugdb/za/description.xml \
    ../global_resources/sql/drugdb/za/licenceterms.txt \
    ../global_resources/sql/drugdb/ca/description.xml \
    ../global_resources/sql/drugdb/ca/licenceterms.txt \
    ../global_resources/sql/drugdb/us/description.xml \
    ../global_resources/sql/drugdb/us/licenceterms.txt \
    ../global_resources/sql/drugdb/za/description.xml \
    ../global_resources/sql/drugdb/za/licenceterms.txt \
    ../global_resources/sql/drugdb/za/za_uids.csv \
    ../global_resources/sql/drugdb/pt/description.xml \
    ../global_resources/sql/drugdb/pt/licenceterms.txt \
    ../global_resources/sql/drugdb/pt/pt_uids.csv \
    ../global_resources/nonfree/ddi/atc_utf8.csv \
    ../global_resources/nonfree/ddi/thesaurus_interactions.xml \
    ../global_resources/nonfree/ddi/afssaps_links.xml \
    ../global_resources/nonfree/ddi/afssaps_classtree.xml \
    ../global_resources/nonfree/ddi/atc_utf8.csv \
    ../global_resources/nonfree/ddi/afssaps_links.xml \
    ../global_resources/nonfree/ddi/afssaps_classtree.xml \
    ../global_resources/nonfree/ddi/moleculetoatclinker.xml \
    ../global_resources/nonfree/pims/icdlinks.xml \
    ../global_resources/nonfree/pims/pims.xml \
    ../global_resources/sql/druginfodb/biam2/biam_importer_schema.sql \


OTHER_FILES += DrugsDB.pluginspec

# include translations
TRANSLATION_NAME = ftbdrugsdb
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)

