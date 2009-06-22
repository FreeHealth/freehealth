include( checkqtversion.pri )

TEMPLATE = subdirs

CONFIG *= ordered

#SUBDIRS += libs/toolkit \
#           libs/usertoolkit \
#           libs/usertoolkit/usermanager

 SUBDIRS += lib_toolkit \
            lib_usertoolkit \
            usermanager

 lib_toolkit.subdir      = libs/toolkit

 lib_usertoolkit.subdir  = libs/usertoolkit
 lib_usertoolkit.depends = lib_toolkit

 usermanager.subdir      = libs/usertoolkit/usermanager
 usermanager.depends     = lib_toolkit
 usermanager.depends     = lib_usertoolkit

ProjectSettings {
    EDITOR = QMakeProjectEditor
    EXECUTE_RELEASE = bin/FreeMedFormsUserManager
    QT_VERSION = 4.5.0
    EXECUTE_DEBUG = bin/FreeMedFormsUserManager_debug
    BUILDER = GNUMake
    COMPILER = G++
}
