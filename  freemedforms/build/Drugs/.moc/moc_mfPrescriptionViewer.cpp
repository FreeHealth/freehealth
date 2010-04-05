/****************************************************************************
** Meta object code from reading C++ file 'mfPrescriptionViewer.h'
**
** Created: Mon Feb 1 19:13:04 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugswidget/mfPrescriptionViewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfPrescriptionViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__PrescriptionViewer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x05,
      49,   32,   32,   32, 0x05,

 // slots: signature, parameters, type, tag, flags
      66,   32,   32,   32, 0x0a,
      83,   32,   32,   32, 0x0a,
     101,   32,   32,   32, 0x0a,
     110,   32,   32,   32, 0x0a,
     121,   32,   32,   32, 0x0a,
     138,  133,   32,   32, 0x0a,
     164,  133,   32,   32, 0x0a,
     194,   32,   32,   32, 0x0a,
     213,   32,   32,   32, 0x0a,
     230,   32,   32,   32, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__PrescriptionViewer[] = {
    "DrugsWidget::PrescriptionViewer\0\0"
    "saveTriggered()\0printTriggered()\0"
    "clearTriggered()\0removeTriggered()\0"
    "moveUp()\0moveDown()\0sortDrugs()\0item\0"
    "showDrugInfo(QModelIndex)\0"
    "showDosageDialog(QModelIndex)\0"
    "viewInteractions()\0changeDuration()\0"
    "changeDurationTo()\0"
};

const QMetaObject DrugsWidget::PrescriptionViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DrugsWidget__PrescriptionViewer,
      qt_meta_data_DrugsWidget__PrescriptionViewer, 0 }
};

const QMetaObject *DrugsWidget::PrescriptionViewer::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::PrescriptionViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__PrescriptionViewer))
        return static_cast<void*>(const_cast< PrescriptionViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int DrugsWidget::PrescriptionViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: saveTriggered(); break;
        case 1: printTriggered(); break;
        case 2: clearTriggered(); break;
        case 3: removeTriggered(); break;
        case 4: moveUp(); break;
        case 5: moveDown(); break;
        case 6: sortDrugs(); break;
        case 7: showDrugInfo((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: showDosageDialog((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 9: viewInteractions(); break;
        case 10: changeDuration(); break;
        case 11: changeDurationTo(); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void DrugsWidget::PrescriptionViewer::saveTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void DrugsWidget::PrescriptionViewer::printTriggered()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
