/****************************************************************************
** Meta object code from reading C++ file 'globaldrugsmodel.h'
**
** Created: Mon Feb 1 19:11:44 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsbaseplugin/globaldrugsmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'globaldrugsmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsDB__GlobalDrugsModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DrugsDB__GlobalDrugsModel[] = {
    "DrugsDB::GlobalDrugsModel\0\0"
    "updateCachedAvailableDosage()\0"
};

const QMetaObject DrugsDB::GlobalDrugsModel::staticMetaObject = {
    { &QSqlTableModel::staticMetaObject, qt_meta_stringdata_DrugsDB__GlobalDrugsModel,
      qt_meta_data_DrugsDB__GlobalDrugsModel, 0 }
};

const QMetaObject *DrugsDB::GlobalDrugsModel::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsDB::GlobalDrugsModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsDB__GlobalDrugsModel))
        return static_cast<void*>(const_cast< GlobalDrugsModel*>(this));
    return QSqlTableModel::qt_metacast(_clname);
}

int DrugsDB::GlobalDrugsModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSqlTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateCachedAvailableDosage(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
