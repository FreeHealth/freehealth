/****************************************************************************
** Meta object code from reading C++ file 'aboutdialog.h'
**
** Created: Sun Sep 13 10:29:40 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/dialogs/aboutdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'aboutdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__Internal__AboutDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      33,   29,   28,   28, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Core__Internal__AboutDialog[] = {
    "Core::Internal::AboutDialog\0\0cat\0"
    "currentItemChanged(QTreeWidgetItem*)\0"
};

const QMetaObject Core::Internal::AboutDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Core__Internal__AboutDialog,
      qt_meta_data_Core__Internal__AboutDialog, 0 }
};

const QMetaObject *Core::Internal::AboutDialog::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::AboutDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__AboutDialog))
        return static_cast<void*>(const_cast< AboutDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Core::Internal::AboutDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
