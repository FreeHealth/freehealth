/****************************************************************************
** Meta object code from reading C++ file 'settingsdialog.h'
**
** Created: Mon Feb 1 11:52:46 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/dialogs/settingsdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settingsdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__SettingsDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x0a,
      32,   21,   21,   21, 0x08,
      47,   21,   21,   21, 0x08,
      56,   21,   21,   21, 0x08,
      65,   21,   21,   21, 0x08,
      73,   21,   21,   21, 0x08,
      91,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Core__SettingsDialog[] = {
    "Core::SettingsDialog\0\0done(int)\0"
    "pageSelected()\0accept()\0reject()\0"
    "apply()\0restoreDefaults()\0showHelp()\0"
};

const QMetaObject Core::SettingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Core__SettingsDialog,
      qt_meta_data_Core__SettingsDialog, 0 }
};

const QMetaObject *Core::SettingsDialog::metaObject() const
{
    return &staticMetaObject;
}

void *Core::SettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__SettingsDialog))
        return static_cast<void*>(const_cast< SettingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Core::SettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: done((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: pageSelected(); break;
        case 2: accept(); break;
        case 3: reject(); break;
        case 4: apply(); break;
        case 5: restoreDefaults(); break;
        case 6: showHelp(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
