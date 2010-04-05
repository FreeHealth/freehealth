/****************************************************************************
** Meta object code from reading C++ file 'userviewer_p.h'
**
** Created: Mon Feb 1 11:54:55 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/usermanagerplugin/widgets/userviewer_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'userviewer_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UserPlugin__Internal__UserViewerPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      41,   40,   40,   40, 0x0a,
      59,   40,   40,   40, 0x0a,
      91,   40,   40,   40, 0x0a,
     123,   40,   40,   40, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_UserPlugin__Internal__UserViewerPrivate[] = {
    "UserPlugin::Internal::UserViewerPrivate\0"
    "\0on_modelReseted()\0on_languageCombo_activated(int)\0"
    "on_but_changePassword_clicked()\0"
    "on_but_viewHistory_clicked()\0"
};

const QMetaObject UserPlugin::Internal::UserViewerPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UserPlugin__Internal__UserViewerPrivate,
      qt_meta_data_UserPlugin__Internal__UserViewerPrivate, 0 }
};

const QMetaObject *UserPlugin::Internal::UserViewerPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *UserPlugin::Internal::UserViewerPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UserPlugin__Internal__UserViewerPrivate))
        return static_cast<void*>(const_cast< UserViewerPrivate*>(this));
    if (!strcmp(_clname, "Ui::UserViewer"))
        return static_cast< Ui::UserViewer*>(const_cast< UserViewerPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int UserPlugin::Internal::UserViewerPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_modelReseted(); break;
        case 1: on_languageCombo_activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: on_but_changePassword_clicked(); break;
        case 3: on_but_viewHistory_clicked(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
