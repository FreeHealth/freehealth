/****************************************************************************
** Meta object code from reading C++ file 'helpdialog.h'
**
** Created: Mon Feb 1 11:52:48 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/dialogs/helpdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'helpdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__HelpDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      38,   17,   17,   17, 0x08,
      56,   51,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Core__HelpDialog[] = {
    "Core::HelpDialog\0\0updateWindowTitle()\0"
    "fullScreen()\0item\0treeActivated(QTreeWidgetItem*)\0"
};

const QMetaObject Core::HelpDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Core__HelpDialog,
      qt_meta_data_Core__HelpDialog, 0 }
};

const QMetaObject *Core::HelpDialog::metaObject() const
{
    return &staticMetaObject;
}

void *Core::HelpDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__HelpDialog))
        return static_cast<void*>(const_cast< HelpDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Core::HelpDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateWindowTitle(); break;
        case 1: fullScreen(); break;
        case 2: treeActivated((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
