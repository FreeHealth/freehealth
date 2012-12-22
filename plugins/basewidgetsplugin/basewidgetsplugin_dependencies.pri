include( ../coreplugin/coreplugin.pri )
include( ../texteditorplugin/texteditorplugin.pri )
include( ../formmanagerplugin/formmanagerplugin.pri )
include( ../patientbaseplugin/patientbaseplugin.pri )
include( ../identityplugin/identityplugin.pri )
QT *= script
isEqual(QT_MAJOR_VERSION, 5) {
    QT *= uitools
} else {
    CONFIG *= uitools
}
