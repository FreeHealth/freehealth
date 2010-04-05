/****************************************************************************
** Meta object code from reading C++ file 'debugdialog.h'
**
** Created: Sun Sep 13 10:29:41 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/dialogs/debugdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'debugdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__Internal__DebugDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      33,   29,   28,   28, 0x08,
      76,   70,   28,   28, 0x08,
     111,   28,   28,   28, 0x08,
     141,   28,   28,   28, 0x08,
     162,   28,   28,   28, 0x08,
     188,   28,  183,   28, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Core__Internal__DebugDialog[] = {
    "Core::Internal::DebugDialog\0\0cat\0"
    "currentItemChanged(QTreeWidgetItem*)\0"
    "pages\0setPages(const QList<IDebugPage*>)\0"
    "on_fullScreenButton_clicked()\0"
    "on_butSend_clicked()\0onSendMessage_done()\0"
    "bool\0on_butSave_clicked()\0"
};

const QMetaObject Core::Internal::DebugDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Core__Internal__DebugDialog,
      qt_meta_data_Core__Internal__DebugDialog, 0 }
};

const QMetaObject *Core::Internal::DebugDialog::metaObject() const
{
    return &staticMetaObject;
}

void *Core::Internal::DebugDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__Internal__DebugDialog))
        return static_cast<void*>(const_cast< DebugDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Core::Internal::DebugDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 1: setPages((*reinterpret_cast< const QList<IDebugPage*>(*)>(_a[1]))); break;
        case 2: on_fullScreenButton_clicked(); break;
        case 3: on_butSend_clicked(); break;
        case 4: onSendMessage_done(); break;
        case 5: { bool _r = on_butSave_clicked();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
