/****************************************************************************
** Meta object code from reading C++ file 'mfInteractionDialog.h'
**
** Created: Mon Feb 1 19:13:06 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugswidget/mfInteractionDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfInteractionDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__InteractionDialog[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      42,   41,   41,   41, 0x08,
      67,   41,   41,   41, 0x08,
      91,   41,   41,   41, 0x08,
     111,   41,   41,   41, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__InteractionDialog[] = {
    "DrugsWidget::Internal::InteractionDialog\0"
    "\0on_printButton_clicked()\0"
    "on_helpButton_clicked()\0on_zoomIn_clicked()\0"
    "on_zoomOut_clicked()\0"
};

const QMetaObject DrugsWidget::Internal::InteractionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__InteractionDialog,
      qt_meta_data_DrugsWidget__Internal__InteractionDialog, 0 }
};

const QMetaObject *DrugsWidget::Internal::InteractionDialog::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::InteractionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__InteractionDialog))
        return static_cast<void*>(const_cast< InteractionDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DrugsWidget::Internal::InteractionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_printButton_clicked(); break;
        case 1: on_helpButton_clicked(); break;
        case 2: on_zoomIn_clicked(); break;
        case 3: on_zoomOut_clicked(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
