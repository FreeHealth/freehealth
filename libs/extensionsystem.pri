# include path for toolkit
include( extensionsystem_dependencies.pri )
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lExtensionSystem_debug
    win32:LIBS   *= -lExtensionSystem_d
} else {
    LIBS  *= -lExtensionSystem
}
