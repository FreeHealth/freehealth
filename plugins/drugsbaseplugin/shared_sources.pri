DEFINES += DRUGSBASE_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/drugsbaseplugin_dependencies.pri )

HEADERS += $${PWD}/drugsbaseplugin.h \
    $${PWD}/drugsbase_exporter.h \
    $${PWD}/drugbaseessentials.h \
    $${PWD}/drugsbase.h \
    $${PWD}/interactionmanager.h \
    $${PWD}/constants.h \
    $${PWD}/drugsmodel.h \
    $${PWD}/dosagemodel.h \
    $${PWD}/drugsio.h \
    $${PWD}/globaldrugsmodel.h \
    $${PWD}/dailyschememodel.h \
    $${PWD}/versionupdater.h \
    $${PWD}/drugstemplateprinter.h \
    $${PWD}/drugsdatabaseselector.h \
    $${PWD}/atctreemodel.h \
    $${PWD}/drugsearchengine.h \
    $${PWD}/constants_databaseschema.h \
    $${PWD}/idrugengine.h \
    $${PWD}/idruginteraction.h \
    $${PWD}/idrug.h \
    $${PWD}/drugbasecore.h \
    $${PWD}/protocolsbase.h \
    $${PWD}/druginteractionquery.h \
    $${PWD}/idrugprescription.h \
    $${PWD}/ivirtualdrug.h \
    $${PWD}/druginteractionresult.h \
    $${PWD}/idruginteractionalert.h \
    $${PWD}/druginteractioninformationquery.h \
    $${PWD}/iprescription.h \
    $${PWD}/prescriptiontoken.h \
    $${PWD}/constants_html.h \
    $${PWD}/prescriptionprinter.h


SOURCES += $${PWD}/drugsbaseplugin.cpp \
    $${PWD}/drugbaseessentials.cpp \
    $${PWD}/drugsbase.cpp \
    $${PWD}/interactionmanager.cpp \
    $${PWD}/drugsmodel.cpp \
    $${PWD}/dosagemodel.cpp \
    $${PWD}/drugsio.cpp \
    $${PWD}/globaldrugsmodel.cpp \
    $${PWD}/dailyschememodel.cpp \
    $${PWD}/versionupdater.cpp \
    $${PWD}/drugstemplateprinter.cpp \
    $${PWD}/drugsdatabaseselector.cpp \
    $${PWD}/atctreemodel.cpp \
    $${PWD}/drugsearchengine.cpp \
    $${PWD}/idrug.cpp \
    $${PWD}/drugbasecore.cpp \
    $${PWD}/protocolsbase.cpp \
    $${PWD}/idruginteractionalert.cpp \
    $${PWD}/druginteractionquery.cpp \
    $${PWD}/druginteractionresult.cpp \
    $${PWD}/druginteractioninformationquery.cpp \
    $${PWD}/idruginteraction.cpp \
    $${PWD}/prescriptiontoken.cpp \
    $${PWD}/prescriptionprinter.cpp

# include translations
TRANSLATION_NAME = drugsbase
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
