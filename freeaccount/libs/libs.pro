# Qt 5.1 cache system
qt:greaterThan(QT_MAJOR_VERSION, 4) {
    cache()
}

TEMPLATE = subdirs

SUBDIRS += translation \
    aggregation \
    extension \
    utils \
    calendar \
    medical \
#    medintux \
    quazip \
    datapack

LIBSPATH=../../libs
CONTRIBPATH=../../contrib

aggregation.subdir  = $${LIBSPATH}/aggregation
aggregation.depends =

extension.subdir    = $${LIBSPATH}/extensionsystem
extension.depends   += aggregation

translation.subdir  = $${LIBSPATH}/translationutils
translation.depends =

utils.subdir        = $${LIBSPATH}/utils
utils.depends       += translation

calendar.subdir     = $${LIBSPATH}/calendar
calendar.depends    += utils
calendar.depends    += extension

medical.subdir      = $${LIBSPATH}/medicalutils

# medintux.subdir     = $${LIBSPATH}/medintuxutils
# medintux.depends    += utils
# medintux.depends    += translation

quazip.subdir       = $${CONTRIBPATH}/quazip
quazip.depends      += utils

datapack.subdir     = $${LIBSPATH}/datapackutils
datapack.depends    += utils
datapack.depends    += translation
datapack.depends    += quazip
