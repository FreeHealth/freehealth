/****************************************************************************
** Meta object code from reading C++ file 'actionmanager_p.h'
**
** Created: Sun Sep 13 10:29:33 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/actionmanager/actionmanager_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'actionmanager_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__Internal__ActionManagerPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      38,   37,   37,   37, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Core__Internal__ActionManagerPrivate[] = {
    "Core::Internal::ActionManagerPrivate\0"
    "\0retranslateMenusAndActions()\0"
};

const QMetaObject Core::Internal::ActionManagerPrivate::staticMetaObject = {
    { &Core::ActionManager::staticMetaObject, qt_meta_stringdata_Core__Internal__ActionManagerPrivate,
      qt_meta_data_Core__Internal__ActionManagerPrivate, 0 }
};

const QMetaObject *Core::Internal::ActionManagerPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::ActionManagerPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__ActionManagerPrivate))
        return static_cast<void*>(const_cast< ActionManagerPrivate*>(this));
    typedef Core::ActionManager QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Core::Internal::ActionManagerPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::ActionManager QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: retranslateMenusAndActions(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
