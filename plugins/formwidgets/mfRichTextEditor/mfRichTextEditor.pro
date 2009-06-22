TARGET = mf_richtextwidgets

include( ../formwidgets.pri )
include( $${PACKAGE_LIBS_SOURCES}/sdk_toolkit.pri )

DESTDIR =  $$FMF_PLUGINS_DIR

SOURCES +=   mfRichTextEditorWidget.cpp
HEADERS +=   mfRichTextEditorWidget.h
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/richtextwidget_fr.ts \
                $${FMF_GLOBAL_RESOURCES}/translations/richtextwidget_de.ts
