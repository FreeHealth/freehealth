include($${SOURCES_PLUGINS_PATH}/coreplugin/coreplugin.pri)

# HPRIM integrator dependencies
contains(DEFINES, WITH_FRENCH_HPRIM_INTEGRATOR){
    include($${SOURCES_PLUGINS_PATH}/patientbaseplugin/patientbaseplugin.pri)
    include($${SOURCES_PLUGINS_PATH}/listviewplugin/listviewplugin.pri)
}

QT *= xml
