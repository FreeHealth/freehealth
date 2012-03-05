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

