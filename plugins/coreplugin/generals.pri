# SOURCES and HEADERS needed by context manager
HEADERS += $${PWD}/core_exporter.h \
    $${PWD}/constants.h \
#    $${PWD}/constants_datapack.h \
    $${PWD}/constants_icons.h \
    $${PWD}/constants_menus.h \
    $${PWD}/constants_tokensandsettings.h \
    $${PWD}/constants_trans.h \
    $${PWD}/icore.h \
    $${PWD}/icorelistener.h \
    $${PWD}/itheme.h \
    $${PWD}/iphotoprovider.h \
    $${PWD}/theme.h \
    $${PWD}/translators.h \
    $${PWD}/isettings.h \
    $${PWD}/settings_p.h \
    $${PWD}/idebugpage.h \
    $${PWD}/iaboutpage.h \
    $${PWD}/ioptionspage.h \
    $${PWD}/imainwindow.h \
    $${PWD}/filemanager.h \
    $${PWD}/ipatient.h \
    $${PWD}/iuser.h \
    $${PWD}/idocumentprinter.h \
    $${PWD}/icommandline.h \
    $${PWD}/ipadtools.h \
    $${PWD}/icalendar.h \
    $${PWD}/ifirstconfigurationpage.h \
    $${PWD}/ipatientlistener.h \
    $${PWD}/igenericpage.h \
    $${PWD}/iscriptmanager.h \
    $${PWD}/constants_colors.h \
    $${PWD}/id.h \
    $${PWD}/ipatientbar.h \
    $${PWD}/ipatientdataexporter.h \
    $${PWD}/applicationautolock.h \
#    $${PWD}/applicationversioninformation.h

SOURCES += \
    $${PWD}/icore.cpp \
    $${PWD}/theme.cpp \
    $${PWD}/translators.cpp \
    $${PWD}/settings.cpp \
    $${PWD}/imainwindow.cpp \
    $${PWD}/filemanager.cpp \
    $${PWD}/ipatient.cpp \
    $${PWD}/iuser.cpp \
    $${PWD}/idocumentprinter.cpp \
    $${PWD}/ipadtools.cpp \
    $${PWD}/igenericpage.cpp \
    $${PWD}/id.cpp \
    $${PWD}/ipatientdataexporter.cpp \
    $${PWD}/applicationautolock.cpp \
#    $${PWD}/applicationversioninformation.cpp
