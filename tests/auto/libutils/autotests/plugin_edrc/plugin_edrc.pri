PATH_TO_PLUGIN = ../../../../freedrc/plugins/edrcplugin

INCLUDEPATH += ../../../../freedrc/plugins

SOURCES += \
    $${PATH_TO_PLUGIN}/consultresultvalidator.cpp \
    $${PATH_TO_PLUGIN}/consultresult.cpp \
    $${PATH_TO_PLUGIN}/database/edrcbase.cpp \
    plugin_edrc/tst_edrcvalidator.cpp

HEADERS += \
    $${PATH_TO_PLUGIN}/edrc_exporter.h \
    $${PATH_TO_PLUGIN}/consultresultvalidator.h \
    $${PATH_TO_PLUGIN}/consultresult.h \
    $${PATH_TO_PLUGIN}/database/edrcbase.h \
    $${PATH_TO_PLUGIN}/database/constants_db.h \
    $${PATH_TO_PLUGIN}/constants.h
