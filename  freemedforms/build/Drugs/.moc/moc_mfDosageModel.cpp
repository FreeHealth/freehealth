/****************************************************************************
** Meta object code from reading C++ file 'mfDosageModel.h'
**
** Created: Sun Sep 13 10:32:55 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugsmodel/mfDosageModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfDosageModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Drugs__Internal__DosageModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      35,   29,   30,   29, 0x0a,
      53,   47,   29,   29, 0x08,
      74,   29,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Drugs__Internal__DosageModel[] = {
    "Drugs::Internal::DosageModel\0\0bool\0"
    "submitAll()\0event\0changeEvent(QEvent*)\0"
    "retranslate()\0"
};

const QMetaObject Drugs::Internal::DosageModel::staticMetaObject = {
    { &QSqlTableModel::staticMetaObject, qt_meta_stringdata_Drugs__Internal__DosageModel,
      qt_meta_data_Drugs__Internal__DosageModel, 0 }
};

const QMetaObject *Drugs::Internal::DosageModel::metaObject() const
{
    return &staticMetaObject;
}

void *Drugs::Internal::DosageModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Drugs__Internal__DosageModel))
        return static_cast<void*>(const_cast< DosageModel*>(this));
    return QSqlTableModel::qt_metacast(_clname);
}

int Drugs::Internal::DosageModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
