/****************************************************************************
** Meta object code from reading C++ file 'lineeditechoswitcher.h'
**
** Created: Sun Sep 13 10:27:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/utils/widgets/lineeditechoswitcher.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lineeditechoswitcher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__LineEditEchoSwitcher[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       1,   17, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      29,   28,   28,   28, 0x0a,

 // properties: name, type, flags
      54,   46, 0x0a195103,

       0        // eod
};

static const char qt_meta_stringdata_Utils__LineEditEchoSwitcher[] = {
    "Utils::LineEditEchoSwitcher\0\0"
    "toogleEchoMode()\0QString\0text\0"
};

const QMetaObject Utils::LineEditEchoSwitcher::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Utils__LineEditEchoSwitcher,
      qt_meta_data_Utils__LineEditEchoSwitcher, 0 }
};

const QMetaObject *Utils::LineEditEchoSwitcher::metaObject() const
{
    return &staticMetaObject;
}

void *Utils::LineEditEchoSwitcher::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__LineEditEchoSwitcher))
        return static_cast<void*>(const_cast< LineEditEchoSwitcher*>(this));
    return QWidget::qt_metacast(_clname);
}

int Utils::LineEditEchoSwitcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: toogleEchoMode(); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setText(*reinterpret_cast< QString*>(_v)); break;
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
