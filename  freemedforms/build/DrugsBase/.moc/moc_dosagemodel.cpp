/****************************************************************************
** Meta object code from reading C++ file 'dosagemodel.h'
**
** Created: Mon Feb 1 19:11:42 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsbaseplugin/dosagemodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dosagemodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsDB__Internal__DosageModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      37,   31,   32,   31, 0x0a,
      55,   49,   31,   31, 0x08,
      76,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsDB__Internal__DosageModel[] = {
    "DrugsDB::Internal::DosageModel\0\0bool\0"
    "submitAll()\0event\0changeEvent(QEvent*)\0"
    "retranslate()\0"
};

const QMetaObject DrugsDB::Internal::DosageModel::staticMetaObject = {
    { &QSqlTableModel::staticMetaObject, qt_meta_stringdata_DrugsDB__Internal__DosageModel,
      qt_meta_data_DrugsDB__Internal__DosageModel, 0 }
};

const QMetaObject *DrugsDB::Internal::DosageModel::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsDB::Internal::DosageModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsDB__Internal__DosageModel))
        return static_cast<void*>(const_cast< DosageModel*>(this));
    return QSqlTableModel::qt_metacast(_clname);
}

int DrugsDB::Internal::DosageModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSqlTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = submitAll();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: changeEvent((*reinterpret_cast< QEvent*(*)>(_a[1]))); break;
        case 2: retranslate(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
