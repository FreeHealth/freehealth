/****************************************************************************
** Meta object code from reading C++ file 'coreimpl.h'
**
** Created: Mon Feb 1 11:53:05 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/fmfcoreplugin/coreimpl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'coreimpl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__Internal__CoreImpl[] = {

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

static const char qt_meta_stringdata_Core__Internal__CoreImpl[] = {
    "Core::Internal::CoreImpl\0"
};

const QMetaObject Core::Internal::CoreImpl::staticMetaObject = {
    { &Core::ICore::staticMetaObject, qt_meta_stringdata_Core__Internal__CoreImpl,
      qt_meta_data_Core__Internal__CoreImpl, 0 }
};

const QMetaObject *Core::Internal::CoreImpl::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::CoreImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__CoreImpl))
        return static_cast<void*>(const_cast< CoreImpl*>(this));
    typedef Core::ICore QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Core::Internal::CoreImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::ICore QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
