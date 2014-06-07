# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

CAMELCASE_APPNAME=UnitTests
include(../../../buildspecs/define_appname.pri)
