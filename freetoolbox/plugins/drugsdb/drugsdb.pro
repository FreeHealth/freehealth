TEMPLATE = lib
TARGET = DrugsDB

include(../../../plugins/fmf_plugins.pri)
include(drugsdb_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    canadiandrugsdatabase.cpp \
    fdadrugsdatabasecreator.cpp \
    frenchdrugsdatabasecreator.cpp \
    extramoleculelinkermodel.cpp \
    moleculelinkerwidget.cpp \
    atcmodel.cpp \
    atcpage.cpp \
    southafricandrugsdatabase.cpp \
    belgishdrugsdatabase.cpp \
    drugsdbplugin.cpp \
    searchatcindatabasedialog.cpp \
    drug.cpp \
    portuguesedrugsdatabase.cpp \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.cpp \


HEADERS  += \
    canadiandrugsdatabase.h \
    fdadrugsdatabasecreator.h \
    frenchdrugsdatabasecreator.h \
    extramoleculelinkermodel.h \
    moleculelinkerwidget.h \
    atcmodel.h \
    atcpage.h \
    southafricandrugsdatabase.h \
    belgishdrugsdatabase.h \
    drugsdbplugin.h \
    searchatcindatabasedialog.h \
    drug.h \
    portuguesedrugsdatabase.h \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.h



FORMS += \
    canadiandrugsdatabasewidget.ui \
    fdadrugsdatabasewidget.ui \
    frenchdrugsdatabasewidget.ui \
    moleculelinkerwidget.ui \
    atcpage.ui \
    southafricandrugsdatabase.ui \
    belgishdrugsdatabase.ui \
    searchatcindatabasedialog.ui \
    portuguesedrugsdatabase.ui

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


OTHER_FILES += DrugsDB.pluginspec


