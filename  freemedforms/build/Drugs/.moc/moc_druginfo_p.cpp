/****************************************************************************
** Meta object code from reading C++ file 'druginfo_p.h'
**
** Created: Mon Feb 1 19:13:01 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugswidget/druginfo_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'druginfo_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DrugInfoPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      40,   39,   39,   39, 0x0a,
      64,   39,   39,   39, 0x0a,
      88,   39,   39,   39, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugInfoPrivate[] = {
    "DrugsWidget::Internal::DrugInfoPrivate\0"
    "\0on_butSendINN_clicked()\0"
    "on_butIAMSend_clicked()\0"
    "on_listWidgetInteractions_itemSelectionChanged()\0"
};

const QMetaObject DrugsWidget::Internal::DrugInfoPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugInfoPrivate,
      qt_meta_data_DrugsWidget__Internal__DrugInfoPrivate, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugInfoPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugInfoPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugInfoPrivate))
        return static_cast<void*>(const_cast< DrugInfoPrivate*>(this));
    if (!strcmp(_clname, "Ui::DrugInfo"))
        return static_cast< Ui::DrugInfo*>(const_cast< DrugInfoPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugInfoPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_butSendINN_clicked(); break;
        case 1: on_butIAMSend_clicked(); break;
        case 2: on_listWidgetInteractions_itemSelectionChanged(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
