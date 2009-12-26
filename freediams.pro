include ( checkqtversion.pri )
TEMPLATE = subdirs
CONFIG *= ordered

SUBDIRS += \
           freediams/libs \
           freediams/plugins \
           freediams \

ProjectSettings { 
    EDITOR = QMakeProjectEditor
    QT_VERSION = 4.5
    BUILDER = GNUMake
    COMPILER = G++
}

OTHER_FILES += update-di.txt \
               README \
               COPYING \
               INSTALL \
               global_resources/textfiles/freediams.desktop \
               global_resources/textfiles/freediams_win32_icon.rc
