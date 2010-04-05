/****************************************************************************
** Meta object code from reading C++ file 'printerpreviewer_p.h'
**
** Created: Tue Feb 9 13:55:27 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/printerplugin/printerpreviewer_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'printerpreviewer_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Print__Internal__PrinterPreviewerPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      42,   41,   41,   41, 0x08,
      86,   75,   41,   41, 0x08,
     134,  128,   41,   41, 0x08,
     176,  170,   41,   41, 0x08,
     215,   41,   41,   41, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Print__Internal__PrinterPreviewerPrivate[] = {
    "Print::Internal::PrinterPreviewerPrivate\0"
    "\0on_updatePreviewButton_clicked()\0"
    "checkstate\0on_automaticUpdateCheck_stateChanged(int)\0"
    "state\0on_duplicataCheck_stateChanged(int)\0"
    "value\0on_pageNumberSpinBox_valueChanged(int)\0"
    "on_tooglePreviewButton_clicked()\0"
};

const QMetaObject Print::Internal::PrinterPreviewerPrivate::staticMetaObject = {
    { &PrinterPreviewer::staticMetaObject, qt_meta_stringdata_Print__Internal__PrinterPreviewerPrivate,
      qt_meta_data_Print__Internal__PrinterPreviewerPrivate, 0 }
};

const QMetaObject *Print::Internal::PrinterPreviewerPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *Print::Internal::PrinterPreviewerPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Print__Internal__PrinterPreviewerPrivate))
        return static_cast<void*>(const_cast< PrinterPreviewerPrivate*>(this));
    return PrinterPreviewer::qt_metacast(_clname);
}

int Print::Internal::PrinterPreviewerPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PrinterPreviewer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_updatePreviewButton_clicked(); break;
        case 1: on_automaticUpdateCheck_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: on_duplicataCheck_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: on_pageNumberSpinBox_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: on_tooglePreviewButton_clicked(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
