/****************************************************************************
** Meta object code from reading C++ file 'mfDosageViewer.h'
**
** Created: Mon Feb 1 19:12:54 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/dosagedialog/mfDosageViewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfDosageViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DosageViewer[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      39,   37,   36,   36, 0x0a,
      49,   36,   36,   36, 0x0a,
      75,   65,   36,   36, 0x0a,
     102,   97,   36,   36, 0x0a,
     138,  132,   36,   36, 0x08,
     178,  132,   36,   36, 0x08,
     221,  219,   36,   36, 0x08,
     261,  219,   36,   36, 0x08,
     302,   36,   36,   36, 0x08,
     331,  132,   36,   36, 0x08,
     373,  132,   36,   36, 0x08,
     403,   36,   36,   36, 0x08,
     432,   36,   36,   36, 0x08,
     471,  465,   36,   36, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DosageViewer[] = {
    "DrugsWidget::Internal::DosageViewer\0"
    "\0r\0done(int)\0commitToModel()\0dosageRow\0"
    "changeCurrentRow(int)\0item\0"
    "changeCurrentRow(QModelIndex)\0state\0"
    "on_fromToIntakesCheck_stateChanged(int)\0"
    "on_fromToDurationCheck_stateChanged(int)\0"
    "d\0on_intakesFromSpin_valueChanged(double)\0"
    "on_durationFromSpin_valueChanged(double)\0"
    "on_userformsButton_clicked()\0"
    "on_dosageForAllInnCheck_stateChanged(int)\0"
    "on_aldCheck_stateChanged(int)\0"
    "on_frenchRCPButton_clicked()\0"
    "on_tabWidget_currentChanged(int)\0index\0"
    "onDailySchemeModelDataChanged(QModelIndex)\0"
};

const QMetaObject DrugsWidget::Internal::DosageViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DosageViewer,
      qt_meta_data_DrugsWidget__Internal__DosageViewer, 0 }
};

const QMetaObject *DrugsWidget::Internal::DosageViewer::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DosageViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DosageViewer))
        return static_cast<void*>(const_cast< DosageViewer*>(this));
    if (!strcmp(_clname, "Ui::DosageViewer"))
        return static_cast< Ui::DosageViewer*>(const_cast< DosageViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int DrugsWidget::Internal::DosageViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: done((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: commitToModel(); break;
        case 2: changeCurrentRow((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 3: changeCurrentRow((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: on_fromToIntakesCheck_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: on_fromToDurationCheck_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: on_intakesFromSpin_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: on_durationFromSpin_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: on_userformsButton_clicked(); break;
        case 9: on_dosageForAllInnCheck_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: on_aldCheck_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: on_frenchRCPButton_clicked(); break;
        case 12: on_tabWidget_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: onDailySchemeModelDataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
