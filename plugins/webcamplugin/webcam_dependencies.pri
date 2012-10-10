include($${SOURCES_PLUGINS_PATH}/coreplugin/coreplugin.pri)

# Add the opencv dependency here
LIBS +=-lopencv_core \
    -lopencv_highgui \
    -lopencv_objdetect \
#    -lopencv_calib3d \
#    -lopencv_contrib \
#    -lopencv_features2d \
#    -lopencv_flann \
#    -lopencv_gpu \
#    -lopencv_imgproc \
#    -lopencv_legacy \
#    -lopencv_ml \
#    -lopencv_video \
