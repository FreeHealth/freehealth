# SOURCES and HEADERS needed by rich text editor
SOURCES += $${PWD}/tkRichTextEditor.cpp \
           $${PWD}/tkRichTextActionHandler.cpp \
           $${PWD}/tkRichTextTableEditor.cpp \
           $${PWD}/tkRichTextEditorManager.cpp

HEADERS += $${PWD}/tkRichTextEditor.h \
           $${PWD}/tkRichTextActionHandler.h \
           $${PWD}/tkRichTextTableEditor.h \
           $${PWD}/tkRichTextEditorContext.h \
           $${PWD}/tkTableDialog.h \
           $${PWD}/tkRichTextEditorManager.h

FORMS += ../../libs/toolkit/richtexteditor/tkTableDialog.ui
