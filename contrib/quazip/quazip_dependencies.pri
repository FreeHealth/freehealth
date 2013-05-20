include($${SOURCES_LIBS_PATH}/utils.pri)

win32|macx {
  DEPENDPATH += $${PWD}/zlib-1.2.8
  INCLUDEPATH += $${PWD}/zlib-1.2.8
}
