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
    widgets/packprocessdialog.h \
#    serverandpackmodel.h \
    iserverengine.h \
    serverengines/localserverengine.h \
    serverengines/httpserverengine.h \
    packmodel.h \
    servermodel.h \
    categorylistwidget.h \
    packdependencychecker.h \
    ipackmanager.h \
    packmanager.h \
    serverenginestatus.h

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
    widgets/packprocessdialog.cpp \
#    serverandpackmodel.cpp \
    iserverengine.cpp \
    serverengines/localserverengine.cpp \
    serverengines/httpserverengine.cpp \
    packmodel.cpp \
    servermodel.cpp \
    categorylistwidget.cpp \
    packdependencychecker.cpp \
    packmanager.cpp

FORMS += \
    widgets/serverpackeditor.ui \
    widgets/addserverdialog.ui \
    widgets/packprocessdialog.ui
















































