include($${SOURCES_PLUGINS_PATH}/coreplugin/coreplugin.pri)

# Add the opencv dependency here
LIBS +=-lopencv_core -lopencv_highgui
