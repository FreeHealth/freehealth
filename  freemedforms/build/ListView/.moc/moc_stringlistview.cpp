/****************************************************************************
** Meta object code from reading C++ file 'stringlistview.h'
**
** Created: Mon Feb 1 11:54:08 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/listviewplugin/stringlistview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stringlistview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Views__StringListView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      31,   22, 0xff195103,
      42,   22, 0xff195103,

       0        // eod
};

static const char qt_meta_stringdata_Views__StringListView[] = {
    "Views::StringListView\0QVariant\0"
    "stringList\0checkedStringList\0"
};

const QMetaObject Views::StringListView::staticMetaObject = {
    { &ListView::staticMetaObject, qt_meta_stringdata_Views__StringListView,
      qt_meta_data_Views__StringListView, 0 }
};

const QMetaObject *Views::StringListView::metaObject() const
{
    return &staticMetaObject;
}

void *Views::StringListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Views__StringListView))
        return static_cast<void*>(const_cast< StringListView*>(this));
    return ListView::qt_metacast(_clname);
}

int Views::StringListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariant*>(_v) = getStringList(); break;
        case 1: *reinterpret_cast< QVariant*>(_v) = getCheckedStringList(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setStringList(*reinterpret_cast< QVariant*>(_v)); break;
        case 1: setCheckedStringList(*reinterpret_cast< QVariant*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
