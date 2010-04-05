/****************************************************************************
** Meta object code from reading C++ file 'stringlistmodel.h'
**
** Created: Sun Sep 13 10:31:07 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/listviewplugin/stringlistmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stringlistmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Views__StringListModel[] = {

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

static const char qt_meta_stringdata_Views__StringListModel[] = {
    "Views::StringListModel\0"
};

const QMetaObject Views::StringListModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_Views__StringListModel,
      qt_meta_data_Views__StringListModel, 0 }
};

const QMetaObject *Views::StringListModel::metaObject() const
{
    return &staticMetaObject;
}

void *Views::StringListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Views__StringListModel))
        return static_cast<void*>(const_cast< StringListModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int Views::StringListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
