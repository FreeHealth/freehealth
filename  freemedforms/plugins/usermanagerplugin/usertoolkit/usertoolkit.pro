# usertoolkit library project file
TEMPLATE         = lib
TARGET           = usertoolkit
PACKAGE_VERSION  = 0.1.0
#!win32:VERSION   = 0.1.0

# include config file
include( ../libsworkbench.pri )
include( ../sdk_toolkit.pri )

# include sdk
INCLUDEPATH *= $${PWD}/sdk

# create a lib
# CONFIG *= staticlib
# DESTDIR = $${PACKAGE_BUILD_PATH}
PRE_TARGETDEPS *= ../toolkit
include( ../rpath.pri )

# define Qt
QT *= sql \
    network
CONFIG *= dll plugin

# make library exportable
DEFINES *= USERTOOLKIT_CORE_BUILD

# Compile
HEADERS += tkUserGlobal.h \
    objects/tkUserExporter.h \
    objects/tkUser.h \
    database/tkUserBase.h \
    widgets/tkUserViewer.h \
    widgets/tkUserViewer_p.h \
    widgets/tkUserIdentifier.h \
    widgets/tkUserIdentifier_p.h \
    widgets/tkUserManager.h \
    widgets/tkUserManager_p.h \
    tkUserConstants.h \
    objects/tkUserModel.h \
    widgets/tkUserPasswordDialog.h \
    widgets/tkUserPasswordDialog_p.h \
    widgets/tkUserRightsWidget.h \
    widgets/tkUserWizard.h
SOURCES += tkUserGlobal.cpp \
    objects/tkUser.cpp \
    database/tkUserBase.cpp \
    widgets/tkUserViewer.cpp \
    widgets/tkUserIdentifier.cpp \
    widgets/tkUserManager.cpp \
    objects/tkUserModel.cpp \
    widgets/tkUserPasswordDialog.cpp \
    widgets/tkUserRightsWidget.cpp \
    widgets/tkUserWizard.cpp
FORMS += widgets/tkUserViewer.ui \
    widgets/tkUserIdentifier.ui \
    widgets/tkUserManager.ui \
    widgets/tkHeadersFootersViewerPrivate.ui \
    widgets/tkUserPasswordDialog.ui

# transltors
TRANSLATIONS += $${FMF_GLOBAL_RESOURCES}/translations/usertoolkit_fr.ts \
    $${FMF_GLOBAL_RESOURCES}/translations/usertoolkit_de.ts
