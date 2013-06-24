PATH_TO_PLUGIN = ../../../../freedrc/plugins/edrcplugin

SOURCES += \
    $${PATH_TO_PLUGIN}/consultresultvalidator.cpp \
    $${PATH_TO_PLUGIN}/consultresult.cpp \
    plugin_edrc/tst_edrcvalidator.cpp

HEADERS += \
    $${PATH_TO_PLUGIN}/edrc_exporter.h \
    $${PATH_TO_PLUGIN}/consultresultvalidator.h \
    $${PATH_TO_PLUGIN}/consultresult.h \
    $${PATH_TO_PLUGIN}/constants.h
