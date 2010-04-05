/****************************************************************************
** Meta object code from reading C++ file 'listview_p.h'
**
** Created: Mon Feb 1 11:54:07 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/listviewplugin/listview_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'listview_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Views__Internal__ListViewActionHandler[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      40,   39,   39,   39, 0x08,
      49,   39,   39,   39, 0x08,
      60,   39,   39,   39, 0x08,
      70,   39,   39,   39, 0x08,
      83,   39,   39,   39, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Views__Internal__ListViewActionHandler[] = {
    "Views::Internal::ListViewActionHandler\0"
    "\0moveUp()\0moveDown()\0addItem()\0"
    "removeItem()\0listViewItemChanged()\0"
};

const QMetaObject Views::Internal::ListViewActionHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Views__Internal__ListViewActionHandler,
      qt_meta_data_Views__Internal__ListViewActionHandler, 0 }
};

const QMetaObject *Views::Internal::ListViewActionHandler::metaObject() const
{
    return &staticMetaObject;
}

void *Views::Internal::ListViewActionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Views__Internal__ListViewActionHandler))
        return static_cast<void*>(const_cast< ListViewActionHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Views::Internal::ListViewActionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: moveUp(); break;
        case 1: moveDown(); break;
        case 2: addItem(); break;
        case 3: removeItem(); break;
        case 4: listViewItemChanged(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_Views__Internal__ListViewManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      41,   34,   33,   33, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Views__Internal__ListViewManager[] = {
    "Views::Internal::ListViewManager\0\0"
    "object\0updateContext(Core::IContext*)\0"
};

const QMetaObject Views::Internal::ListViewManager::staticMetaObject = {
    { &ListViewActionHandler::staticMetaObject, qt_meta_stringdata_Views__Internal__ListViewManager,
      qt_meta_data_Views__Internal__ListViewManager, 0 }
};

const QMetaObject *Views::Internal::ListViewManager::metaObject() const
{
    return &staticMetaObject;
}

void *Views::Internal::ListViewManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Views__Internal__ListViewManager))
        return static_cast<void*>(const_cast< ListViewManager*>(this));
    return ListViewActionHandler::qt_metacast(_clname);
}

int Views::Internal::ListViewManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ListViewActionHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateContext((*reinterpret_cast< Core::IContext*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
