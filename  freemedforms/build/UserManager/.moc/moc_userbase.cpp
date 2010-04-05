/****************************************************************************
** Meta object code from reading C++ file 'userbase.h'
**
** Created: Mon Feb 1 11:54:53 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/usermanagerplugin/database/userbase.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'userbase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UserPlugin__Internal__UserBase[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_UserPlugin__Internal__UserBase[] = {
    "UserPlugin::Internal::UserBase\0"
};

const QMetaObject UserPlugin::Internal::UserBase::staticMetaObject = {
    { &Utils::Database::staticMetaObject, qt_meta_stringdata_UserPlugin__Internal__UserBase,
      qt_meta_data_UserPlugin__Internal__UserBase, 0 }
};

const QMetaObject *UserPlugin::Internal::UserBase::metaObject() const
{
    return &staticMetaObject;
}

void *UserPlugin::Internal::UserBase::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UserPlugin__Internal__UserBase))
        return static_cast<void*>(const_cast< UserBase*>(this));
    typedef Utils::Database QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int UserPlugin::Internal::UserBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Utils::Database QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
