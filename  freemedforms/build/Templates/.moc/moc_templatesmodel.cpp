/****************************************************************************
** Meta object code from reading C++ file 'templatesmodel.h'
**
** Created: Mon Feb 1 19:10:31 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/templatesplugin/templatesmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'templatesmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Templates__TemplatesModel[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      32,   26,   27,   26, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Templates__TemplatesModel[] = {
    "Templates::TemplatesModel\0\0bool\0"
    "submit()\0"
};

const QMetaObject Templates::TemplatesModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_Templates__TemplatesModel,
      qt_meta_data_Templates__TemplatesModel, 0 }
};

const QMetaObject *Templates::TemplatesModel::metaObject() const
{
    return &staticMetaObject;
}

void *Templates::TemplatesModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Templates__TemplatesModel))
        return static_cast<void*>(const_cast< TemplatesModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int Templates::TemplatesModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = submit();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
