include( ../coreplugin/coreplugin.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri)
include( $${SOURCES_LIBS_PATH}/translationutils.pri)

# FreeMedForms only inclusion
isEmpty(FREEDIAMS):include( ../formmanagerplugin/formmanagerplugin.pri )
