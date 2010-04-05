# check qt version : at least >= 4.4 for debian release
TOO_OLD_LIST=$$find(QT_VERSION, ^4\.[0-3])
count(TOO_OLD_LIST, 1) {
    message( " ******  Cannot build the Qt Creator with a Qt version that old:" $$QT_VERSION )
    error( "***** Use at least Qt 4.5. *****")
}
