TEMPLATE = lib
TARGET = DrugsDB
PACKAGE_VERSION = 0.0.2

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
    canadianlocalizeddrugsdatabase.cpp \
    fdadrugsdatabasecreator.cpp \
    frenchdrugsdatabasecreator.cpp \
    extramoleculelinkermodel.cpp \
    moleculelinkerwidget.cpp \
    atcmodel.cpp \
    atcpage.cpp \
    southafricandrugsdatabase.cpp \
    belgishdrugsdatabase.cpp \
    drugsdbplugin.cpp \
    searchatcindatabasedialog.cpp


HEADERS  += \
    canadiandrugsdatabase.h \
    canadianlocalizeddrugsdatabase.h \
    fdadrugsdatabasecreator.h \
    frenchdrugsdatabasecreator.h \
    extramoleculelinkermodel.h \
    moleculelinkerwidget.h \
    atcmodel.h \
    atcpage.h \
    southafricandrugsdatabase.h \
    belgishdrugsdatabase.h \
    drugsdbplugin.h \
    searchatcindatabasedialog.h


FORMS += \
    canadiandrugsdatabasewidget.ui \
    canadianlocalizeddrugsdatabase.ui \
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
    ../global_resources/sql/za_db_finalize.sql \
    ../global_resources/sql/canadian_db_creator.sql \
    ../global_resources/sql/canadian_db_preparation.sql \
    ../global_resources/sql/canadian_db_imports.sql \
    ../global_resources/sql/canadian_db_finalize.sql \
    ../global_resources/sql/usa_db_creator.sql \
    ../global_resources/sql/usa_db_finalize.sql \
    ../global_resources/sql/atc_utf8.csv \
    ../global_resources/sql/create-fr.sql \
    ../global_resources/sql/create-fr2.sql


OTHER_FILES += DrugsDB.pluginspec
