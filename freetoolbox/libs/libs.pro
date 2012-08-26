TEMPLATE = subdirs
CONFIG *= ordered

LIB_PATH=../../libs

SUBDIRS += \
           $${LIB_PATH}/translationutils \
           $${LIB_PATH}/aggregation \
           $${LIB_PATH}/extensionsystem \
           $${LIB_PATH}/utils \
           $${LIB_PATH}/medicalutils \
           $${LIB_PATH}/../contrib/quazip \
           $${LIB_PATH}/datapackutils \

