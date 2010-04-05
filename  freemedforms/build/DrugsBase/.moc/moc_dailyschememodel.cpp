/****************************************************************************
** Meta object code from reading C++ file 'dailyschememodel.h'
**
** Created: Tue Feb 9 13:56:49 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsbaseplugin/dailyschememodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dailyschememodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsDB__DailySchemeModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_DrugsDB__DailySchemeModel[] = {
    "DrugsDB::DailySchemeModel\0\0methodChanged()\0"
};

const QMetaObject DrugsDB::DailySchemeModel::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_DrugsDB__DailySchemeModel,
      qt_meta_data_DrugsDB__DailySchemeModel, 0 }
};

const QMetaObject *DrugsDB::DailySchemeModel::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsDB::DailySchemeModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsDB__DailySchemeModel))
        return static_cast<void*>(const_cast< DailySchemeModel*>(this));
    return QAbstractTableModel::qt_metacast(_clname);
}

int DrugsDB::DailySchemeModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: methodChanged(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DrugsDB::DailySchemeModel::methodChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
