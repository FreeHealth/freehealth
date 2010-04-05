/****************************************************************************
** Meta object code from reading C++ file 'QButtonLineEdit.h'
**
** Created: Sun Sep 13 10:27:55 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/utils/widgets/QButtonLineEdit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QButtonLineEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__QButtonLineEdit[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      31,   24,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Utils__QButtonLineEdit[] = {
    "Utils::QButtonLineEdit\0\0action\0"
    "leftTrig(QAction*)\0"
};

const QMetaObject Utils::QButtonLineEdit::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_Utils__QButtonLineEdit,
      qt_meta_data_Utils__QButtonLineEdit, 0 }
};

const QMetaObject *Utils::QButtonLineEdit::metaObject() const
{
    return &staticMetaObject;
}

void *Utils::QButtonLineEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__QButtonLineEdit))
        return static_cast<void*>(const_cast< QButtonLineEdit*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int Utils::QButtonLineEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: leftTrig((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
