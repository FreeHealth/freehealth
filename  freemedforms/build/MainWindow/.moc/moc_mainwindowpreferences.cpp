/****************************************************************************
** Meta object code from reading C++ file 'mainwindowpreferences.h'
**
** Created: Mon Feb 1 19:09:29 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/fmfmainwindowplugin/mainwindowpreferences.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindowpreferences.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWin__Internal__MainWindowPreferencesWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      50,   48,   47,   47, 0x0a,
      83,   47,   47,   47, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_MainWin__Internal__MainWindowPreferencesWidget[] = {
    "MainWin::Internal::MainWindowPreferencesWidget\0"
    "\0s\0saveToSettings(Core::ISettings*)\0"
    "saveToSettings()\0"
};

const QMetaObject MainWin::Internal::MainWindowPreferencesWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MainWin__Internal__MainWindowPreferencesWidget,
      qt_meta_data_MainWin__Internal__MainWindowPreferencesWidget, 0 }
};

const QMetaObject *MainWin::Internal::MainWindowPreferencesWidget::metaObject() const
{
    return &staticMetaObject;
}

void *MainWin::Internal::MainWindowPreferencesWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWin__Internal__MainWindowPreferencesWidget))
        return static_cast<void*>(const_cast< MainWindowPreferencesWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int MainWin::Internal::MainWindowPreferencesWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_MainWin__Internal__MainWindowPreferencesPage[] = {

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

static const char qt_meta_stringdata_MainWin__Internal__MainWindowPreferencesPage[] = {
    "MainWin::Internal::MainWindowPreferencesPage\0"
};

const QMetaObject MainWin::Internal::MainWindowPreferencesPage::staticMetaObject = {
    { &Core::IOptionsPage::staticMetaObject, qt_meta_stringdata_MainWin__Internal__MainWindowPreferencesPage,
      qt_meta_data_MainWin__Internal__MainWindowPreferencesPage, 0 }
};

const QMetaObject *MainWin::Internal::MainWindowPreferencesPage::metaObject() const
{
    return &staticMetaObject;
}

void *MainWin::Internal::MainWindowPreferencesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWin__Internal__MainWindowPreferencesPage))
        return static_cast<void*>(const_cast< MainWindowPreferencesPage*>(this));
    typedef Core::IOptionsPage QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int MainWin::Internal::MainWindowPreferencesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IOptionsPage QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
