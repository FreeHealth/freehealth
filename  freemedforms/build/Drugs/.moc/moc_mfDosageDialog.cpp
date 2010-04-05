/****************************************************************************
** Meta object code from reading C++ file 'mfDosageDialog.h'
**
** Created: Mon Feb 1 19:12:50 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/dosagedialog/mfDosageDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfDosageDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DosageDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      39,   37,   36,   36, 0x08,
      49,   36,   36,   36, 0x08,
      77,   36,   36,   36, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DosageDialog[] = {
    "DrugsWidget::Internal::DosageDialog\0"
    "\0r\0done(int)\0on_drugNameButton_clicked()\0"
    "on_innButton_clicked()\0"
};

const QMetaObject DrugsWidget::Internal::DosageDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DosageDialog,
      qt_meta_data_DrugsWidget__Internal__DosageDialog, 0 }
};

const QMetaObject *DrugsWidget::Internal::DosageDialog::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DosageDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DosageDialog))
        return static_cast<void*>(const_cast< DosageDialog*>(this));
    if (!strcmp(_clname, "Ui::DosageDialog"))
        return static_cast< Ui::DosageDialog*>(const_cast< DosageDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DrugsWidget::Internal::DosageDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: done((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: on_drugNameButton_clicked(); break;
        case 2: on_innButton_clicked(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
