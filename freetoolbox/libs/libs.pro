TEMPLATE = subdirs
CONFIG *= ordered

LIB_PATH=../../libs

SUBDIRS += \
           ../../contrib/quazip \
           $${LIB_PATH}/translationutils \
           $${LIB_PATH}/aggregation \
           $${LIB_PATH}/extensionsystem \
           $${LIB_PATH}/utils \
           $${LIB_PATH}/medicalutils \
           $${LIB_PATH}/datapackutils \
