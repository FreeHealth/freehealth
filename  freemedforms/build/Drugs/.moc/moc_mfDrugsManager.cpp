/****************************************************************************
** Meta object code from reading C++ file 'mfDrugsManager.h'
**
** Created: Sun Sep 13 10:33:09 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/mfDrugsManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfDrugsManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Drugs__Internal__DrugsActionHandler[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      37,   36,   36,   36, 0x0a,
      57,   36,   36,   36, 0x08,
      66,   36,   36,   36, 0x08,
      77,   36,   36,   36, 0x08,
      89,   36,   36,   36, 0x08,
     102,   36,   36,   36, 0x08,
     110,   36,   36,   36, 0x08,
     129,   36,   36,   36, 0x08,
     153,  151,   36,   36, 0x08,
     183,   36,   36,   36, 0x08,
     203,   36,   36,   36, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Drugs__Internal__DrugsActionHandler[] = {
    "Drugs::Internal::DrugsActionHandler\0"
    "\0drugsModelChanged()\0moveUp()\0moveDown()\0"
    "sortDrugs()\0removeItem()\0clear()\0"
    "viewInteractions()\0listViewItemChanged()\0"
    "a\0searchActionChanged(QAction*)\0"
    "printPrescription()\0toogleTestingDrugs()\0"
};

const QMetaObject Drugs::Internal::DrugsActionHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Drugs__Internal__DrugsActionHandler,
      qt_meta_data_Drugs__Internal__DrugsActionHandler, 0 }
};

const QMetaObject *Drugs::Internal::DrugsActionHandler::metaObject() const
{
    return &staticMetaObject;
}

void *Drugs::Internal::DrugsActionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Drugs__Internal__DrugsActionHandler))
        return static_cast<void*>(const_cast< DrugsActionHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Drugs::Internal::DrugsActionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: drugsModelChanged(); break;
        case 1: moveUp(); break;
        case 2: moveDown(); break;
        case 3: sortDrugs(); break;
        case 4: removeItem(); break;
        case 5: clear(); break;
        case 6: viewInteractions(); break;
        case 7: listViewItemChanged(); break;
        case 8: searchActionChanged((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 9: printPrescription(); break;
        case 10: toogleTestingDrugs(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}
static const uint qt_meta_data_Drugs__Internal__DrugsManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      38,   31,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Drugs__Internal__DrugsManager[] = {
    "Drugs::Internal::DrugsManager\0\0object\0"
    "updateContext(Core::IContext*)\0"
};

const QMetaObject Drugs::Internal::DrugsManager::staticMetaObject = {
    { &DrugsActionHandler::staticMetaObject, qt_meta_stringdata_Drugs__Internal__DrugsManager,
      qt_meta_data_Drugs__Internal__DrugsManager, 0 }
};

const QMetaObject *Drugs::Internal::DrugsManager::metaObject() const
{
    return &staticMetaObject;
}

void *Drugs::Internal::DrugsManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Drugs__Internal__DrugsManager))
        return static_cast<void*>(const_cast< DrugsManager*>(this));
    return DrugsActionHandler::qt_metacast(_clname);
}

int Drugs::Internal::DrugsManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrugsActionHandler::qt_metacall(_c, _id, _a);
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
