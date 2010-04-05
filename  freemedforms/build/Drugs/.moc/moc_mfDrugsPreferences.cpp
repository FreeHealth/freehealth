/****************************************************************************
** Meta object code from reading C++ file 'mfDrugsPreferences.h'
**
** Created: Mon Feb 1 19:12:57 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugspreferences/mfDrugsPreferences.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfDrugsPreferences.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DrugsViewWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      42,   40,   39,   39, 0x0a,
      75,   39,   39,   39, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugsViewWidget[] = {
    "DrugsWidget::Internal::DrugsViewWidget\0"
    "\0s\0saveToSettings(Core::ISettings*)\0"
    "saveToSettings()\0"
};

const QMetaObject DrugsWidget::Internal::DrugsViewWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugsViewWidget,
      qt_meta_data_DrugsWidget__Internal__DrugsViewWidget, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugsViewWidget::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugsViewWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugsViewWidget))
        return static_cast<void*>(const_cast< DrugsViewWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugsViewWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: saveToSettings((*reinterpret_cast< Core::ISettings*(*)>(_a[1]))); break;
        case 1: saveToSettings(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_DrugsWidget__Internal__DrugsPrintWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      43,   41,   40,   40, 0x0a,
      76,   40,   40,   40, 0x2a,
      93,   40,   40,   40, 0x0a,
     120,   40,   40,   40, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugsPrintWidget[] = {
    "DrugsWidget::Internal::DrugsPrintWidget\0"
    "\0s\0saveToSettings(Core::ISettings*)\0"
    "saveToSettings()\0resetToDefaultFormatting()\0"
    "updateFormatting()\0"
};

const QMetaObject DrugsWidget::Internal::DrugsPrintWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugsPrintWidget,
      qt_meta_data_DrugsWidget__Internal__DrugsPrintWidget, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugsPrintWidget::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugsPrintWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugsPrintWidget))
        return static_cast<void*>(const_cast< DrugsPrintWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugsPrintWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: saveToSettings((*reinterpret_cast< Core::ISettings*(*)>(_a[1]))); break;
        case 1: saveToSettings(); break;
        case 2: resetToDefaultFormatting(); break;
        case 3: updateFormatting(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_DrugsWidget__Internal__DrugsUserWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      42,   40,   39,   39, 0x0a,
      75,   39,   39,   39, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugsUserWidget[] = {
    "DrugsWidget::Internal::DrugsUserWidget\0"
    "\0s\0saveToSettings(Core::ISettings*)\0"
    "saveToSettings()\0"
};

const QMetaObject DrugsWidget::Internal::DrugsUserWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugsUserWidget,
      qt_meta_data_DrugsWidget__Internal__DrugsUserWidget, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugsUserWidget::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugsUserWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugsUserWidget))
        return static_cast<void*>(const_cast< DrugsUserWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugsUserWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: saveToSettings((*reinterpret_cast< Core::ISettings*(*)>(_a[1]))); break;
        case 1: saveToSettings(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_DrugsWidget__Internal__DrugsExtraWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      43,   41,   40,   40, 0x0a,
      76,   40,   40,   40, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugsExtraWidget[] = {
    "DrugsWidget::Internal::DrugsExtraWidget\0"
    "\0s\0saveToSettings(Core::ISettings*)\0"
    "saveToSettings()\0"
};

const QMetaObject DrugsWidget::Internal::DrugsExtraWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugsExtraWidget,
      qt_meta_data_DrugsWidget__Internal__DrugsExtraWidget, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugsExtraWidget::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugsExtraWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugsExtraWidget))
        return static_cast<void*>(const_cast< DrugsExtraWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugsExtraWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: saveToSettings((*reinterpret_cast< Core::ISettings*(*)>(_a[1]))); break;
        case 1: saveToSettings(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
