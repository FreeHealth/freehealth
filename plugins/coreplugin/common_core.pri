# include sources common to FreeMedForms and its derivatives for the coreplugins

# include spec installer
include(../fmf_plugins.pri)

# include libs SDKs
include(coreplugin_dependencies.pri)

# define exporter
DEFINES += CORE_LIBRARY

# Qt definitions
QT *= xml \
    network \
    sql

include(actionmanager/actionmanager.pri)
include(modemanager/modemanager.pri)
include(contextmanager/contextmanager.pri)
include(dialogs/dialogs.pri)
include(generals.pri)

OTHER_FILES += $${_PRO_FILE_PWD_}/Core.pluginspec

