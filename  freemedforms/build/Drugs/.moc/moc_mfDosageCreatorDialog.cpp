/****************************************************************************
** Meta object code from reading C++ file 'mfDosageCreatorDialog.h'
**
** Created: Mon Feb 1 19:12:52 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/dosagedialog/mfDosageCreatorDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfDosageCreatorDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DosageCreatorDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      46,   44,   43,   43, 0x08,
      56,   43,   43,   43, 0x08,
      80,   43,   43,   43, 0x08,
     109,   43,   43,   43, 0x08,
     145,   43,   43,   43, 0x08,
     169,   43,   43,   43, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DosageCreatorDialog[] = {
    "DrugsWidget::Internal::DosageCreatorDialog\0"
    "\0r\0done(int)\0on_saveButton_clicked()\0"
    "on_prescribeButton_clicked()\0"
    "on_saveAndPrescribeButton_clicked()\0"
    "on_helpButton_clicked()\0"
    "on_testOnlyButton_clicked()\0"
};

const QMetaObject DrugsWidget::Internal::DosageCreatorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DosageCreatorDialog,
      qt_meta_data_DrugsWidget__Internal__DosageCreatorDialog, 0 }
};

const QMetaObject *DrugsWidget::Internal::DosageCreatorDialog::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DosageCreatorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DosageCreatorDialog))
        return static_cast<void*>(const_cast< DosageCreatorDialog*>(this));
    if (!strcmp(_clname, "Ui::DosageCreatorDialog"))
        return static_cast< Ui::DosageCreatorDialog*>(const_cast< DosageCreatorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DrugsWidget::Internal::DosageCreatorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: done((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: on_saveButton_clicked(); break;
        case 2: on_prescribeButton_clicked(); break;
        case 3: on_saveAndPrescribeButton_clicked(); break;
        case 4: on_helpButton_clicked(); break;
        case 5: on_testOnlyButton_clicked(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
