# Prepare the user's manual for freediams

# This suppose that doxygen is installed on your OS
# On MacOs : Doxygen must be installed as bundle into the Applications directory
# On Linux : Doxygen must be installed from apt-get
# On Windows --> tests at all...

# Docs output are copied into $${FMF_GLOBAL_RESOURCES}/doc/application
#
#
macx{
    DOXYGEN = $$system(locate Doxygen.app/Contents/Resources/doxygen)
    HELPGENERATOR = $$[QT_INSTALL_BINS]/qhelpgenerator
    message(macos : $${DOXYGEN} $${HELPGENERATOR})
}else:unix{
    DOXYGEN = $$system(which doxygen)
    HELPGENERATOR = $$[QT_INSTALL_BINS]\qhelpgenerator
    message(unix : $${DOXYGEN} $${HELPGENERATOR})
} else:win32{
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
IN_DOXYFILE_EN = $${PWD}/usermanual/freediams/Doxyfile_en
en_html_docs.commands = $$DOXYGEN $${IN_DOXYFILE_EN}
QMAKE_EXTRA_TARGETS += en_html_docs

IN_DOXYFILE_FR = $${PWD}/usermanual/freediams/Doxyfile_fr
fr_html_docs.commands = $$DOXYGEN $${IN_DOXYFILE_FR}
QMAKE_EXTRA_TARGETS += fr_html_docs

OTHER_FILES += $${PWD}/usermanual/freediams/freediams.qdoc \
               $${PWD}/usermanual/versionning.qdoc \
               $${PWD}/usermanual/drugswidget/drugswidget.qdoc \
               $${PWD}/usermanual/drugswidget/drugsdatabaseschema.qdoc \
               $${PWD}/usermanual/richtexteditor/richtexteditor.qdoc \
               $${PWD}/usermanual/tokensmanager/tokens.qdoc \
               $${PWD}/usermanual/templates/templates.qdoc \
               $${PWD}/usermanual/freediams/Doxyfile_en \
               $${PWD}/usermanual/freediams/Doxyfile_fr \
               $${PWD}/usermanual/freediams/doxygen.css \
               $${PWD}/usermanual/freediams/footer.html
