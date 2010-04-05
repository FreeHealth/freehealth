/****************************************************************************
** Meta object code from reading C++ file 'updatechecker.h'
**
** Created: Mon Feb 1 11:51:21 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/utils/updatechecker.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'updatechecker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__UpdateChecker[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      33,   22,   21,   21, 0x05,
      54,   21,   21,   21, 0x05,
      68,   21,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
      79,   21,   21,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Utils__UpdateChecker[] = {
    "Utils::UpdateChecker\0\0updateText\0"
    "updateFound(QString)\0updateFound()\0"
    "done(bool)\0showUpdateInformations()\0"
};

const QMetaObject Utils::UpdateChecker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Utils__UpdateChecker,
      qt_meta_data_Utils__UpdateChecker, 0 }
};

const QMetaObject *Utils::UpdateChecker::metaObject() const
{
    return &staticMetaObject;
}

void *Utils::UpdateChecker::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__UpdateChecker))
        return static_cast<void*>(const_cast< UpdateChecker*>(this));
    return QObject::qt_metacast(_clname);
}

int Utils::UpdateChecker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateFound((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: updateFound(); break;
        case 2: done((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: showUpdateInformations(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Utils::UpdateChecker::updateFound(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Utils::UpdateChecker::updateFound()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Utils::UpdateChecker::done(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
