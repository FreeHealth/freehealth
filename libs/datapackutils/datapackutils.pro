# toolkit library project file
TEMPLATE        = lib
TARGET          = DataPack

# include config file
include(../libsworkbench.pri)
include(../datapack_dependencies.pri)

DEFINES *= DATAPACKUTILS_LIBRARY

HEADERS += datapack_exporter.h \
    servermanager.h \
    server.h \
    serverdescription.h \
    packdescription.h \
    pack.h \
    serveridentification.h \
    servercontent.h \
    datapackcore.h \
    iservermanager.h \
    widgets/serverpackeditor.h \
    widgets/addserverdialog.h \
    iserverengine.h \
    serverengines/localserverengine.h \
    serverengines/httpserverengine.h \
    packmodel.h \
    servermodel.h \
    categorylistwidget.h \
    packdependencychecker.h \
    ipackmanager.h \
    packmanager.h \
    serverenginestatus.h \
    widgets/packwizard.h

SOURCES += \
    servermanager.cpp \
    server.cpp \
    serverdescription.cpp \
    packdescription.cpp \
    pack.cpp \
    serveridentification.cpp \
    servercontent.cpp \
    datapackcore.cpp \
    iservermanager.cpp \
    widgets/serverpackeditor.cpp \
    widgets/addserverdialog.cpp \
    iserverengine.cpp \
    serverengines/localserverengine.cpp \
    serverengines/httpserverengine.cpp \
    packmodel.cpp \
    servermodel.cpp \
    categorylistwidget.cpp \
    packdependencychecker.cpp \
    packmanager.cpp \
    widgets/packwizard.cpp

FORMS += \
    widgets/serverpackeditor.ui \
    widgets/addserverdialog.ui

# translators
TRANSLATIONS += $${SOURCES_TRANSLATIONS_PATH}/datapackutils_fr.ts \
                $${SOURCES_TRANSLATIONS_PATH}/datapackutils_de.ts \
                $${SOURCES_TRANSLATIONS_PATH}/datapackutils_es.ts
