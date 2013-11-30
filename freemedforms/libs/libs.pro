# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

LIBSPATH=../../libs/

CONFIG *= ordered

SUBDIRS += $${LIBSPATH}/translationutils \
    $${LIBSPATH}/aggregation \
    $${LIBSPATH}/extensionsystem \
    $${LIBSPATH}/utils \
    $${LIBSPATH}/calendar \
    $${LIBSPATH}/medicalutils \
    $${LIBSPATH}/medintuxutils \
    $${LIBSPATH}/../contrib/quazip \
    $${LIBSPATH}/datapackutils

