DEFINES += EDITOR_LIBRARY

include($${PWD}/../fmf_plugins.pri)
include($${PWD}/texteditorplugin_dependencies.pri )

HEADERS +=  \
    $${PWD}/texteditor_exporter.h \
    $${PWD}/editoractionhandler.h \
    $${PWD}/editorcontext.h \
    $${PWD}/editormanager.h \
    $${PWD}/tabledialog.h \
    $${PWD}/tableeditor.h \
    $${PWD}/texteditor.h \
    $${PWD}/tablepropertieswidget.h \
    $${PWD}/texteditordialog.h \
    $$PWD/constants.h

SOURCES += \
    $${PWD}/editoractionhandler.cpp \
    $${PWD}/editormanager.cpp \
    $${PWD}/tableeditor.cpp \
    $${PWD}/texteditor.cpp \
    $${PWD}/tablepropertieswidget.cpp \
    $${PWD}/texteditordialog.cpp

FORMS += $${PWD}/tabledialog.ui \
    $${PWD}/tablepropertieswidget.ui \
    $${PWD}/tablepropertiesdialog.ui \
    $${PWD}/texteditordialog.ui

# include translations
TRANSLATION_NAME = texteditor
include($${SOURCES_ROOT_PATH}/buildspecs/translations.pri)
