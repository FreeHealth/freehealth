/****************************************************************************
** Meta object code from reading C++ file 'contextmanager_p.h'
**
** Created: Sun Sep 13 10:29:37 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/contextmanager/contextmanager_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'contextmanager_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__Internal__ContextManagerPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      47,   39,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Core__Internal__ContextManagerPrivate[] = {
    "Core::Internal::ContextManagerPrivate\0"
    "\0old,now\0updateFocusWidget(QWidget*,QWidget*)\0"
};

const QMetaObject Core::Internal::ContextManagerPrivate::staticMetaObject = {
    { &ContextManager::staticMetaObject, qt_meta_stringdata_Core__Internal__ContextManagerPrivate,
      qt_meta_data_Core__Internal__ContextManagerPrivate, 0 }
};

const QMetaObject *Core::Internal::ContextManagerPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::ContextManagerPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__ContextManagerPrivate))
        return static_cast<void*>(const_cast< ContextManagerPrivate*>(this));
    return ContextManager::qt_metacast(_clname);
}

int Core::Internal::ContextManagerPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContextManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateFocusWidget((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
