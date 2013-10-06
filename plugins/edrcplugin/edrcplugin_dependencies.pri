include($${SOURCES_PLUGINS_PATH}/coreplugin/coreplugin.pri)
include($${SOURCES_PLUGINS_PATH}/texteditorplugin/texteditorplugin.pri)
include($${SOURCES_PLUGINS_PATH}/listviewplugin/listviewplugin.pri)

# FreeMedForms only inclusion
isEmpty(FREEDRC):include($${SOURCES_PLUGINS_PATH}/formmanagerplugin/formmanagerplugin.pri)

QT *= sql
