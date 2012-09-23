# This file is preprocess during the qmake pass
# The pre-processing is done in the root project .pro file
# see: define_appname.pri
LOWERED_APPNAME=freediams
DEFINES*=FREEDIAMS
FREEDIAMS=1
BUILD_PATH_POSTFIXE = FreeDiams

# manages extra plugins
with-pad    { DEFINES += WITH_PAD }
with-alerts { DEFINES += WITH_ALERTS }
#with-agenda { DEFINES += WITH_AGENDA }
