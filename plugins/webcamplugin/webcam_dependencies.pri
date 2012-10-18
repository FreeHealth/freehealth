include($${SOURCES_PLUGINS_PATH}/coreplugin/coreplugin.pri)

# Add the opencv dependencies
win32 {
    LIBS +=-lopencv_core242 \
        -lopencv_highgui242 \
        -lopencv_objdetect242
} else {
    LIBS +=-lopencv_core \
        -lopencv_highgui \
        -lopencv_objdetect
}
