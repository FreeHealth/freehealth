# toolkit library project file
TEMPLATE        = lib
TARGET          = DataPackUtils

# The version number is defined by the scripts/source.sh script, keep this line unchanged
# NOT USED
# VERSION=1.0.0

# include config file
include(../libsworkbench.pri)
include(../datapackutils_dependencies.pri)

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
    packcategoriesmodel.h \
    packmodel.h \
    servermodel.h \
    categorylistwidget.h \
    packdependencychecker.h \
    ipackmanager.h \
    packmanager.h \
    serverenginestatus.h \
    widgets/packwizard.h \
    packcreationqueue.h \
    constants.h

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
    packcategoriesmodel.cpp \
    packmodel.cpp \
    servermodel.cpp \
    categorylistwidget.cpp \
    packdependencychecker.cpp \
    packmanager.cpp \
    widgets/packwizard.cpp \
    ipackmanager.cpp \
    packcreationqueue.cpp

FORMS += \
    widgets/serverpackeditor.ui \
    widgets/addserverdialog.ui

# include translations
TRANSLATION_NAME = datapack
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
