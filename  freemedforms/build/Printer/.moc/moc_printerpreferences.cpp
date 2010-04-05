/****************************************************************************
** Meta object code from reading C++ file 'printerpreferences.h'
**
** Created: Tue Feb 9 13:55:29 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/printerplugin/printerpreferences.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'printerpreferences.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Print__Internal__PrinterPreferencesWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      45,   43,   42,   42, 0x0a,
      78,   42,   42,   42, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_Print__Internal__PrinterPreferencesWidget[] = {
    "Print::Internal::PrinterPreferencesWidget\0"
    "\0s\0saveToSettings(Core::ISettings*)\0"
    "saveToSettings()\0"
};

const QMetaObject Print::Internal::PrinterPreferencesWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Print__Internal__PrinterPreferencesWidget,
      qt_meta_data_Print__Internal__PrinterPreferencesWidget, 0 }
};

const QMetaObject *Print::Internal::PrinterPreferencesWidget::metaObject() const
{
    return &staticMetaObject;
}

void *Print::Internal::PrinterPreferencesWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Print__Internal__PrinterPreferencesWidget))
        return static_cast<void*>(const_cast< PrinterPreferencesWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Print::Internal::PrinterPreferencesWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_Print__Internal__PrinterPreferencesPage[] = {

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

static const char qt_meta_stringdata_Print__Internal__PrinterPreferencesPage[] = {
    "Print::Internal::PrinterPreferencesPage\0"
};

const QMetaObject Print::Internal::PrinterPreferencesPage::staticMetaObject = {
    { &Core::IOptionsPage::staticMetaObject, qt_meta_stringdata_Print__Internal__PrinterPreferencesPage,
      qt_meta_data_Print__Internal__PrinterPreferencesPage, 0 }
};

const QMetaObject *Print::Internal::PrinterPreferencesPage::metaObject() const
{
    return &staticMetaObject;
}

void *Print::Internal::PrinterPreferencesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Print__Internal__PrinterPreferencesPage))
        return static_cast<void*>(const_cast< PrinterPreferencesPage*>(this));
    typedef Core::IOptionsPage QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Print::Internal::PrinterPreferencesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IOptionsPage QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
