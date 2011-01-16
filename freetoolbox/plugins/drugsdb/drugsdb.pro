TEMPLATE = lib
TARGET = DrugsDB

DEFINES += FREETOOLBOX
FREETOOLBOX = 1

BUILD_PATH_POSTFIXE = FreeToolBox

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
    drug.cpp


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
    drug.h


FORMS += \
    canadiandrugsdatabasewidget.ui \
    fdadrugsdatabasewidget.ui \
    frenchdrugsdatabasewidget.ui \
    moleculelinkerwidget.ui \
    atcpage.ui \
    southafricandrugsdatabase.ui \
    belgishdrugsdatabase.ui \
    searchatcindatabasedialog.ui

OTHER_FILES += \
    ../global_resources/sql/drugs_schema.sql \
    ../global_resources/sql/moleculetoatclinker.xml \
    ../global_resources/sql/atc_utf8.csv \
    ../global_resources/sql/drugdb/newdbschema.sql \
    ../global_resources/sql/drugdb/be/be_db_finalize.sql \
    ../global_resources/sql/drugdb/fr/fr_db_finalize.sql \
    ../global_resources/sql/drugdb/za/za_db_finalize.sql \
    ../global_resources/sql/drugdb/ca/canadian_db_creator.sql \
    ../global_resources/sql/drugdb/ca/canadian_db_finalize.sql \
    ../global_resources/sql/drugdb/ca/canadian_db_imports.sql \
    ../global_resources/sql/drugdb/ca/canadian_db_processing.sql \
    ../global_resources/sql/drugdb/ca/canadian_db_tables.sql \
    ../global_resources/sql/drugdb/us/us_db_finalize.sql \
    ../global_resources/sql/drugdb/za/za_db_finalize.sql \
    ../global_resources/sql/drugdb/za/za_uids.csv \


OTHER_FILES += DrugsDB.pluginspec
