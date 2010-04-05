/****************************************************************************
** Meta object code from reading C++ file 'updatechecker_p.h'
**
** Created: Mon Feb 1 11:51:19 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/utils/updatechecker_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'updatechecker_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__Internal__UpdateCheckerPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      45,   39,   38,   38, 0x08,
      81,   60,   38,   38, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Utils__Internal__UpdateCheckerPrivate[] = {
    "Utils::Internal::UpdateCheckerPrivate\0"
    "\0error\0httpDone(bool)\0bytesRead,totalBytes\0"
    "updateDataReadProgress(int,int)\0"
};

const QMetaObject Utils::Internal::UpdateCheckerPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Utils__Internal__UpdateCheckerPrivate,
      qt_meta_data_Utils__Internal__UpdateCheckerPrivate, 0 }
};

const QMetaObject *Utils::Internal::UpdateCheckerPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *Utils::Internal::UpdateCheckerPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__Internal__UpdateCheckerPrivate))
        return static_cast<void*>(const_cast< UpdateCheckerPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int Utils::Internal::UpdateCheckerPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: httpDone((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: updateDataReadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
