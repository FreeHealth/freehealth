/****************************************************************************
** Meta object code from reading C++ file 'plugindialog.h'
**
** Created: Sun Sep 13 10:29:42 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/dialogs/plugindialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plugindialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__PluginDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      36,   19,   19,   19, 0x08,
      55,   50,   19,   19, 0x08,
      97,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Core__PluginDialog[] = {
    "Core::PluginDialog\0\0updateButtons()\0"
    "openDetails()\0spec\0"
    "openDetails(ExtensionSystem::PluginSpec*)\0"
    "openErrorDetails()\0"
};

const QMetaObject Core::PluginDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Core__PluginDialog,
      qt_meta_data_Core__PluginDialog, 0 }
};

const QMetaObject *Core::PluginDialog::metaObject() const
{
    return &staticMetaObject;
}

void *Core::PluginDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__PluginDialog))
        return static_cast<void*>(const_cast< PluginDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Core::PluginDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateButtons(); break;
        case 1: openDetails(); break;
        case 2: openDetails((*reinterpret_cast< ExtensionSystem::PluginSpec*(*)>(_a[1]))); break;
        case 3: openErrorDetails(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
