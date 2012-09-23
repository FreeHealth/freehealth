TEMPLATE=subdirs
SUBDIRS= # don't build anything, we're just generating the buildspecs/compiling.pri file

isEmpty(CAMELCASE_APPNAME) {
    error(You must define a CAMELCASE_APPNAME)
}

LOW_APPNAME=$$lower($${CAMELCASE_APPNAME})
UP_APPNAME=$$upper($${CAMELCASE_APPNAME})

PROJECT_APPNAME_FILE=$${PWD}/compiling.pri.in
#OUT_FILE=$${PWD}/compiling.pri
#exists($${OUT_FILE}) {
#    removeold.input = OUT_FILE
#    removeold.commands = $${QMAKE_DEL_FILE} $${OUT_FILE}
#    removeold.output = OUT_FILE
##    QMAKE_EXTRA_COMPILERS += removeold
#}
QMAKE_SUBSTITUTES+=$${PROJECT_APPNAME_FILE}
