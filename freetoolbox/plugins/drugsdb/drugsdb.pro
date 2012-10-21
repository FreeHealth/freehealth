TEMPLATE = lib
TARGET = DrugsDB

include(../../../plugins/fmf_plugins.pri)
include(drugsdb_dependencies.pri)

QT *= sql network xml

INCLUDEPATH += ../
DEPENDPATH += ../

SOURCES += \
    tools.cpp \
    frenchdrugsdatabasecreator.cpp \
#    canadiandrugsdatabase.cpp \
#    fdadrugsdatabasecreator.cpp \
#    southafricandrugsdatabase.cpp \
#    belgishdrugsdatabase.cpp \
#    portuguesedrugsdatabase.cpp \
    extramoleculelinkermodel.cpp \
    moleculelinkerwidget.cpp \
    atcmodel.cpp \
    atcpage.cpp \
    drugsdbplugin.cpp \
    searchatcindatabasedialog.cpp \
    drug.cpp \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.cpp \
    drugdatabasedescription.cpp \
    routesmodel.cpp \
    drugsdbcore.cpp

HEADERS  += \
    tools.h \
    frenchdrugsdatabasecreator.h \
#    canadiandrugsdatabase.h \
#    fdadrugsdatabasecreator.h \
#    southafricandrugsdatabase.h \
#    belgishdrugsdatabase.h \
#    portuguesedrugsdatabase.h \
    extramoleculelinkermodel.h \
    moleculelinkerwidget.h \
    atcmodel.h \
    atcpage.h \
    drugsdbplugin.h \
    searchatcindatabasedialog.h \
    drug.h \
    $${SOURCES_PLUGINS_PATH}/drugsbaseplugin/drugbaseessentials.h \
    drugdatabasedescription.h \
    routesmodel.h \
    drugsdbcore.h

FORMS += \
    frenchdrugsdatabasewidget.ui \
#    canadiandrugsdatabasewidget.ui \
#    fdadrugsdatabasewidget.ui \
#    southafricandrugsdatabase.ui \
#    belgishdrugsdatabase.ui \
#    portuguesedrugsdatabase.ui \
    moleculelinkerwidget.ui \
    atcpage.ui \
    searchatcindatabasedialog.ui

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


