# Qt cache system
cache()

TEMPLATE=subdirs
SUBDIRS= # don't build anything, we're just generating the buildspecs/compiling.pri file

isEmpty(CAMELCASE_APPNAME) {
    error(You must define a CAMELCASE_APPNAME)
}

LOW_APPNAME=$$lower($${CAMELCASE_APPNAME})
UP_APPNAME=$$upper($${CAMELCASE_APPNAME})

COMPIL_VAR=$${PWD}/compiling.pri
COMPIL_VAR_IN=$${COMPIL_VAR}.in
#exists($${COMPIL_VAR}) {
#    removeold.input = COMPIL_VAR
#    removeold.commands = $${QMAKE_DEL_FILE} $${QMAKE_FILE_IN}
#    removeold.output = COMPIL_VAR
#    removeold.name = RM_COMPILVARS
#    removeold.CONFIG += no_link
#    removeold.variable_out = PRE_TARGETDEPS
#    QMAKE_PRE_LINK += removeold
##    QMAKE_PRE_LINK += $${QMAKE_DEL_FILE} $${COMPIL_VAR}
##    QMAKE_EXTRA_COMPILERS += $${QMAKE_DEL_FILE} $${COMPIL_VAR}
#    message($${QMAKE_DEL_FILE} $${COMPIL_VAR})
#}
QMAKE_SUBSTITUTES+=$${COMPIL_VAR_IN}
