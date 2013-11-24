# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

CONFIG *= ordered

SUBDIRS += \
           ../../libs/aggregation \
           ../../libs/extensionsystem \
           ../../libs/translationutils \
           ../../libs/utils \
           ../../libs/medicalutils \
#           ../../contrib/quazip \
#           ../../libs/datapackutils \
#           ../../libs/medintuxutils \

