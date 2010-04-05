/****************************************************************************
** Meta object code from reading C++ file 'userrightswidget.h'
**
** Created: Mon Feb 1 11:55:03 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/usermanagerplugin/widgets/userrightswidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'userrightswidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UserPlugin__Internal__UserRightsWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       1,   17, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      45,   40,   39,   39, 0x08,

 // properties: name, type, flags
     103,   99, 0x02195003,

       0        // eod
};

static const char qt_meta_stringdata_UserPlugin__Internal__UserRightsWidget[] = {
    "UserPlugin::Internal::UserRightsWidget\0"
    "\0item\0on_m_RightsListWidget_itemActivated(QListWidgetItem*)\0"
    "int\0rights\0"
};

const QMetaObject UserPlugin::Internal::UserRightsWidget::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_UserPlugin__Internal__UserRightsWidget,
      qt_meta_data_UserPlugin__Internal__UserRightsWidget, 0 }
};

const QMetaObject *UserPlugin::Internal::UserRightsWidget::metaObject() const
{
    return &staticMetaObject;
}

void *UserPlugin::Internal::UserRightsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UserPlugin__Internal__UserRightsWidget))
        return static_cast<void*>(const_cast< UserRightsWidget*>(this));
    return QListWidget::qt_metacast(_clname);
}

int UserPlugin::Internal::UserRightsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_m_RightsListWidget_itemActivated((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = getRigths(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setRigths(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
