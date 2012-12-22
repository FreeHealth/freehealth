include( ../coreplugin/coreplugin.pri )
include( ../texteditorplugin/texteditorplugin.pri )
include( $${SOURCES_LIBS_PATH}/utils.pri)
include( $${SOURCES_LIBS_PATH}/translationutils.pri)

# Manage Qt >= 5
greaterThan(QT_MAJOR_VERSION, 4): QT *= printsupport
