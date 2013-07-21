include( ../coreplugin/coreplugin.pri )
include( ../drugsbaseplugin/drugsbaseplugin.pri )
include( ../templatesplugin/templatesplugin.pri )
include( ../texteditorplugin/texteditorplugin.pri )
include( ../printerplugin/printerplugin.pri )
include( ../listviewplugin/listviewplugin.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri)
include( $${SOURCES_LIBS_PATH}/translationutils.pri)
include( $${SOURCES_LIBS_PATH}/medicalutils.pri)

# FreeMedForms only inclusion
isEmpty(FREEDIAMS):include( ../formmanagerplugin/formmanagerplugin.pri )
