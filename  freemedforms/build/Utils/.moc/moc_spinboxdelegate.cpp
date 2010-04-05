/****************************************************************************
** Meta object code from reading C++ file 'spinboxdelegate.h'
**
** Created: Mon Feb 1 11:51:23 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/utils/widgets/spinboxdelegate.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spinboxdelegate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__SpinBoxDelegate[] = {

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

static const char qt_meta_stringdata_Utils__SpinBoxDelegate[] = {
    "Utils::SpinBoxDelegate\0"
};

const QMetaObject Utils::SpinBoxDelegate::staticMetaObject = {
    { &QItemDelegate::staticMetaObject, qt_meta_stringdata_Utils__SpinBoxDelegate,
      qt_meta_data_Utils__SpinBoxDelegate, 0 }
};

const QMetaObject *Utils::SpinBoxDelegate::metaObject() const
{
    return &staticMetaObject;
}

void *Utils::SpinBoxDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__SpinBoxDelegate))
        return static_cast<void*>(const_cast< SpinBoxDelegate*>(this));
    return QItemDelegate::qt_metacast(_clname);
}

int Utils::SpinBoxDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
