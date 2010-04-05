/****************************************************************************
** Meta object code from reading C++ file 'drugsio.h'
**
** Created: Mon Feb 1 19:11:43 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsbaseplugin/drugsio.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'drugsio.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsDB__DrugsIO[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsDB__DrugsIO[] = {
    "DrugsDB::DrugsIO\0\0transmissionDone()\0"
    "dosageTransmissionDone()\0"
};

const QMetaObject DrugsDB::DrugsIO::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DrugsDB__DrugsIO,
      qt_meta_data_DrugsDB__DrugsIO, 0 }
};

const QMetaObject *DrugsDB::DrugsIO::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsDB::DrugsIO::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsDB__DrugsIO))
        return static_cast<void*>(const_cast< DrugsIO*>(this));
    return QObject::qt_metacast(_clname);
}

int DrugsDB::DrugsIO::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: transmissionDone(); break;
        case 1: dosageTransmissionDone(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void DrugsDB::DrugsIO::transmissionDone()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
