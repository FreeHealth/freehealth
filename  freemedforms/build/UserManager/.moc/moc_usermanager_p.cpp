/****************************************************************************
** Meta object code from reading C++ file 'usermanager_p.h'
**
** Created: Mon Feb 1 11:55:00 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/usermanagerplugin/widgets/usermanager_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'usermanager_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UserPlugin__Internal__UserManagerPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      42,   41,   41,   41, 0x08,
      78,   74,   41,   41, 0x08,
     120,   41,   41,   41, 0x08,
     143,   41,   41,   41, 0x08,
     183,   41,   41,   41, 0x08,
     215,   41,   41,   41, 0x08,
     252,   41,   41,   41, 0x08,
     281,   41,   41,   41, 0x08,
     302,  299,   41,   41, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UserPlugin__Internal__UserManagerPrivate[] = {
    "UserPlugin::Internal::UserManagerPrivate\0"
    "\0on_searchLineEdit_textchanged()\0act\0"
    "on_m_SearchToolButton_triggered(QAction*)\0"
    "on_saveAct_triggered()\0"
    "on_userTableView_activated(QModelIndex)\0"
    "on_createNewUserAct_triggered()\0"
    "on_clearModificationsAct_triggered()\0"
    "on_deleteUserAct_triggered()\0"
    "updateStatusBar()\0id\0"
    "showUserDebugDialog(QModelIndex)\0"
};

const QMetaObject UserPlugin::Internal::UserManagerPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UserPlugin__Internal__UserManagerPrivate,
      qt_meta_data_UserPlugin__Internal__UserManagerPrivate, 0 }
};

const QMetaObject *UserPlugin::Internal::UserManagerPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *UserPlugin::Internal::UserManagerPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UserPlugin__Internal__UserManagerPrivate))
        return static_cast<void*>(const_cast< UserManagerPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int UserPlugin::Internal::UserManagerPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_searchLineEdit_textchanged(); break;
        case 1: on_m_SearchToolButton_triggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: on_saveAct_triggered(); break;
        case 3: on_userTableView_activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: on_createNewUserAct_triggered(); break;
        case 5: on_clearModificationsAct_triggered(); break;
        case 6: on_deleteUserAct_triggered(); break;
        case 7: updateStatusBar(); break;
        case 8: showUserDebugDialog((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
