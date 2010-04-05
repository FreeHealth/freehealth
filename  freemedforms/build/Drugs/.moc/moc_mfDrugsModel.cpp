/****************************************************************************
** Meta object code from reading C++ file 'mfDrugsModel.h'
**
** Created: Sun Sep 13 10:32:53 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugsmodel/mfDrugsModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfDrugsModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Drugs__Internal__DrugsModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      34,   29,   28,   28, 0x05,
      69,   28,   28,   28, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_Drugs__Internal__DrugsModel[] = {
    "Drugs::Internal::DrugsModel\0\0html\0"
    "prescriptionResultChanged(QString)\0"
    "numberOfRowsChanged()\0"
};

const QMetaObject Drugs::Internal::DrugsModel::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_Drugs__Internal__DrugsModel,
      qt_meta_data_Drugs__Internal__DrugsModel, 0 }
};

const QMetaObject *Drugs::Internal::DrugsModel::metaObject() const
{
    return &staticMetaObject;
}

void *Drugs::Internal::DrugsModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Drugs__Internal__DrugsModel))
        return static_cast<void*>(const_cast< DrugsModel*>(this));
    return QAbstractTableModel::qt_metacast(_clname);
}

int Drugs::Internal::DrugsModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: prescriptionResultChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: numberOfRowsChanged(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Drugs::Internal::DrugsModel::prescriptionResultChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Drugs::Internal::DrugsModel::numberOfRowsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
