/****************************************************************************
** Meta object code from reading C++ file 'formglobalmodel.h'
**
** Created: Tue Feb 9 09:15:51 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/formglobalmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'formglobalmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__FakeDataFactory[] = {

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

static const char qt_meta_stringdata_Core__FakeDataFactory[] = {
    "Core::FakeDataFactory\0"
};

const QMetaObject Core::FakeDataFactory::staticMetaObject = {
    { &Core::IFormItemDataFactory::staticMetaObject, qt_meta_stringdata_Core__FakeDataFactory,
      qt_meta_data_Core__FakeDataFactory, 0 }
};

const QMetaObject *Core::FakeDataFactory::metaObject() const
{
    return &staticMetaObject;
}

void *Core::FakeDataFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__FakeDataFactory))
        return static_cast<void*>(const_cast< FakeDataFactory*>(this));
    typedef Core::IFormItemDataFactory QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Core::FakeDataFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IFormItemDataFactory QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Core__FormGlobalModel[] = {

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

static const char qt_meta_stringdata_Core__FormGlobalModel[] = {
    "Core::FormGlobalModel\0"
};

const QMetaObject Core::FormGlobalModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_Core__FormGlobalModel,
      qt_meta_data_Core__FormGlobalModel, 0 }
};

const QMetaObject *Core::FormGlobalModel::metaObject() const
{
    return &staticMetaObject;
}

void *Core::FormGlobalModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__FormGlobalModel))
        return static_cast<void*>(const_cast< FormGlobalModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int Core::FormGlobalModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
