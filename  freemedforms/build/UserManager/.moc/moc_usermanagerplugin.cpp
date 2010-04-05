/****************************************************************************
** Meta object code from reading C++ file 'usermanagerplugin.h'
**
** Created: Mon Feb 1 11:54:50 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/usermanagerplugin/usermanagerplugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'usermanagerplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UserPlugin__UserManagerPlugin[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      31,   30,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_UserPlugin__UserManagerPlugin[] = {
    "UserPlugin::UserManagerPlugin\0\0"
    "showUserManager()\0"
};

const QMetaObject UserPlugin::UserManagerPlugin::staticMetaObject = {
    { &ExtensionSystem::IPlugin::staticMetaObject, qt_meta_stringdata_UserPlugin__UserManagerPlugin,
      qt_meta_data_UserPlugin__UserManagerPlugin, 0 }
};

const QMetaObject *UserPlugin::UserManagerPlugin::metaObject() const
{
    return &staticMetaObject;
}

void *UserPlugin::UserManagerPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UserPlugin__UserManagerPlugin))
        return static_cast<void*>(const_cast< UserManagerPlugin*>(this));
    typedef ExtensionSystem::IPlugin QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int UserPlugin::UserManagerPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef ExtensionSystem::IPlugin QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showUserManager(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
