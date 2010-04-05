/****************************************************************************
** Meta object code from reading C++ file 'colorbuttonchooser.h'
**
** Created: Mon Feb 1 11:51:22 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/utils/widgets/colorbuttonchooser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colorbuttonchooser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__ColorButtonChooser[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       1,   22, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      33,   27,   26,   26, 0x0a,
      50,   26,   26,   26, 0x09,

 // properties: name, type, flags
      27,   62, 0x43095103,

       0        // eod
};

static const char qt_meta_stringdata_Utils__ColorButtonChooser[] = {
    "Utils::ColorButtonChooser\0\0color\0"
    "setColor(QColor)\0onClicked()\0QColor\0"
};

const QMetaObject Utils::ColorButtonChooser::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_Utils__ColorButtonChooser,
      qt_meta_data_Utils__ColorButtonChooser, 0 }
};

const QMetaObject *Utils::ColorButtonChooser::metaObject() const
{
    return &staticMetaObject;
}

void *Utils::ColorButtonChooser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__ColorButtonChooser))
        return static_cast<void*>(const_cast< ColorButtonChooser*>(this));
    return QPushButton::qt_metacast(_clname);
}

int Utils::ColorButtonChooser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 1: onClicked(); break;
        default: ;
        }
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = color(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setColor(*reinterpret_cast< QColor*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
