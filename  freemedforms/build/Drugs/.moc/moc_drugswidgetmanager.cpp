/****************************************************************************
** Meta object code from reading C++ file 'drugswidgetmanager.h'
**
** Created: Tue Feb 9 13:57:29 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugswidgetmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'drugswidgetmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DrugsActionHandler[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      43,   42,   42,   42, 0x0a,
      63,   42,   42,   42, 0x08,
      72,   42,   42,   42, 0x08,
      83,   42,   42,   42, 0x08,
      95,   42,   42,   42, 0x08,
     108,   42,   42,   42, 0x08,
     116,   42,   42,   42, 0x08,
     135,   42,   42,   42, 0x08,
     159,  157,   42,   42, 0x08,
     189,   42,   42,   42, 0x08,
     209,   42,   42,   42, 0x08,
     230,   42,   42,   42, 0x08,
     247,   42,   42,   42, 0x08,
     264,   42,   42,   42, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugsActionHandler[] = {
    "DrugsWidget::Internal::DrugsActionHandler\0"
    "\0drugsModelChanged()\0moveUp()\0moveDown()\0"
    "sortDrugs()\0removeItem()\0clear()\0"
    "viewInteractions()\0listViewItemChanged()\0"
    "a\0searchActionChanged(QAction*)\0"
    "printPrescription()\0toogleTestingDrugs()\0"
    "changeDuration()\0createTemplate()\0"
    "printPreview()\0"
};

const QMetaObject DrugsWidget::Internal::DrugsActionHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugsActionHandler,
      qt_meta_data_DrugsWidget__Internal__DrugsActionHandler, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugsActionHandler::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugsActionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugsActionHandler))
        return static_cast<void*>(const_cast< DrugsActionHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugsActionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 11: changeDuration(); break;
        case 12: createTemplate(); break;
        case 13: printPreview(); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
static const uint qt_meta_data_DrugsWidget__DrugsWidgetManager[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      40,   33,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__DrugsWidgetManager[] = {
    "DrugsWidget::DrugsWidgetManager\0\0"
    "object\0updateContext(Core::IContext*)\0"
};

const QMetaObject DrugsWidget::DrugsWidgetManager::staticMetaObject = {
    { &Internal::DrugsActionHandler::staticMetaObject, qt_meta_stringdata_DrugsWidget__DrugsWidgetManager,
      qt_meta_data_DrugsWidget__DrugsWidgetManager, 0 }
};

const QMetaObject *DrugsWidget::DrugsWidgetManager::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::DrugsWidgetManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__DrugsWidgetManager))
        return static_cast<void*>(const_cast< DrugsWidgetManager*>(this));
    typedef Internal::DrugsActionHandler QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int DrugsWidget::DrugsWidgetManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Internal::DrugsActionHandler QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
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
