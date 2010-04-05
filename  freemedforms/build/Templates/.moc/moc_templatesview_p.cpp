/****************************************************************************
** Meta object code from reading C++ file 'templatesview_p.h'
**
** Created: Mon Feb 1 19:10:34 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/templatesplugin/templatesview_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'templatesview_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Templates__Internal__TemplatesViewActionHandler[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      49,   48,   48,   48, 0x08,
      63,   48,   48,   48, 0x08,
      76,   48,   48,   48, 0x08,
      94,   48,   48,   48, 0x08,
     106,   48,   48,   48, 0x08,
     114,   48,   48,   48, 0x08,
     121,   48,   48,   48, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Templates__Internal__TemplatesViewActionHandler[] = {
    "Templates::Internal::TemplatesViewActionHandler\0"
    "\0addCategory()\0removeItem()\0"
    "editCurrentItem()\0saveModel()\0print()\0"
    "lock()\0templatesViewItemChanged()\0"
};

const QMetaObject Templates::Internal::TemplatesViewActionHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Templates__Internal__TemplatesViewActionHandler,
      qt_meta_data_Templates__Internal__TemplatesViewActionHandler, 0 }
};

const QMetaObject *Templates::Internal::TemplatesViewActionHandler::metaObject() const
{
    return &staticMetaObject;
}

void *Templates::Internal::TemplatesViewActionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Templates__Internal__TemplatesViewActionHandler))
        return static_cast<void*>(const_cast< TemplatesViewActionHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Templates::Internal::TemplatesViewActionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addCategory(); break;
        case 1: removeItem(); break;
        case 2: editCurrentItem(); break;
        case 3: saveModel(); break;
        case 4: print(); break;
        case 5: lock(); break;
        case 6: templatesViewItemChanged(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
static const uint qt_meta_data_Templates__Internal__TemplatesViewManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      50,   43,   42,   42, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Templates__Internal__TemplatesViewManager[] = {
    "Templates::Internal::TemplatesViewManager\0"
    "\0object\0updateContext(Core::IContext*)\0"
};

const QMetaObject Templates::Internal::TemplatesViewManager::staticMetaObject = {
    { &TemplatesViewActionHandler::staticMetaObject, qt_meta_stringdata_Templates__Internal__TemplatesViewManager,
      qt_meta_data_Templates__Internal__TemplatesViewManager, 0 }
};

const QMetaObject *Templates::Internal::TemplatesViewManager::metaObject() const
{
    return &staticMetaObject;
}

void *Templates::Internal::TemplatesViewManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Templates__Internal__TemplatesViewManager))
        return static_cast<void*>(const_cast< TemplatesViewManager*>(this));
    return TemplatesViewActionHandler::qt_metacast(_clname);
}

int Templates::Internal::TemplatesViewManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TemplatesViewActionHandler::qt_metacall(_c, _id, _a);
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
