/****************************************************************************
** Meta object code from reading C++ file 'formmanager.h'
**
** Created: Mon Feb 1 11:52:59 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/formmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'formmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__FormManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      24,   18,   19,   18, 0x0a,
      46,   41,   19,   18, 0x0a,
      76,   71,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Core__FormManager[] = {
    "Core::FormManager\0\0bool\0translateForms()\0"
    "root\0setFormObjects(QObject*)\0item\0"
    "changeStackedLayoutTo(QTreeWidgetItem*)\0"
};

const QMetaObject Core::FormManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Core__FormManager,
      qt_meta_data_Core__FormManager, 0 }
};

const QMetaObject *Core::FormManager::metaObject() const
{
    return &staticMetaObject;
}

void *Core::FormManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__FormManager))
        return static_cast<void*>(const_cast< FormManager*>(this));
    return QObject::qt_metacast(_clname);
}

int Core::FormManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = translateForms();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { bool _r = setFormObjects((*reinterpret_cast< QObject*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: changeStackedLayoutTo((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
