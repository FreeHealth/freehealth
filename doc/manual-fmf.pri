# Prepare the user's manual for drugsinteractions

# This suppose that doxygen is installed on your OS
# On MacOs : Doxygen must be installed as bundle into the Applications directory
# On Linux : Doxygen must be installed from apt-get
# On Windows --> tests at all...

# Docs output are copied into $${FMF_GLOBAL_RESOURCES}/doc/application
#
#
macx{
    DOXYGEN = /Applications/Developer/Doxygen.app/Contents/Resources/doxygen
    HELPGENERATOR = $$[QT_INSTALL_BINS]/qhelpgenerator
    message(macos : $${DOXYGEN} $${HELPGENERATOR})
}else:unix{
    DOXYGEN = /usr/bin/doxygen
    HELPGENERATOR = $$[QT_INSTALL_BINS]\qhelpgenerator
    message(unix : $${DOXYGEN} $${HELPGENERATOR})
} else:win32 {
    DOXYGEN = doxygen.exe
    HELPGENERATOR = $$[QT_INSTALL_BINS]\qhelpgenerator.exe
    message(unix : $${DOXYGEN} $${HELPGENERATOR})
}

# test results
isEmpty(DOXYGEN) {
  warning(Doxygen not found on your machine.)
} else {
  !exists(DOXYGEN) {
     warning(Doxygen is not installed on your machine. Abording the documentation build)
     warning(Path of Doxygen : $${DOXYGEN})
  }
}
isEmpty(HELPGENERATOR) {
  warning(qhelpgenerator not found on your machine. Abording the documentation build)
}
# else {
#  !exists(HELPGENERATOR) {
#     warning(qhelpgenerator is not installed on your machine. Abording the documentation build)
#     warning(Path of qhelpgenerator : $${HELPGENERATOR})
#  }
#}

# create the HTML output of Doxygen
IN_DOXYFILE = $${PWD}/usermanual/freemedforms/Doxyfile
html_docs.commands = $$DOXYGEN $${IN_DOXYFILE}
QMAKE_EXTRA_TARGETS += html_docs

OTHER_FILES += $${PWD}/usermanual/freemedforms/freemedforms.qdoc \
               $${PWD}/usermanual/usermanager/usermanager.qdoc \
               $${PWD}/usermanual/freemedforms/Doxyfile \
               $${PWD}/usermanual/freemedforms/doxygen.css
