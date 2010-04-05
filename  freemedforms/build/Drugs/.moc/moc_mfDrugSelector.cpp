/****************************************************************************
** Meta object code from reading C++ file 'mfDrugSelector.h'
**
** Created: Tue Feb 9 13:57:32 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugswidget/mfDrugSelector.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfDrugSelector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DrugSelector[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      41,   37,   36,   36, 0x05,
      65,   59,   36,   36, 0x05,

 // slots: signature, parameters, type, tag, flags
      91,   36,   36,   36, 0x0a,
     114,   36,   36,   36, 0x08,
     134,   59,   36,   36, 0x08,
     166,   36,   36,   36, 0x08,
     206,   36,   36,   36, 0x08,
     237,  230,   36,   36, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugSelector[] = {
    "DrugsWidget::Internal::DrugSelector\0"
    "\0CIS\0drugSelected(int)\0index\0"
    "drugSelected(QModelIndex)\0"
    "retranslateUi(QString)\0updateModelFilter()\0"
    "on_InnView_clicked(QModelIndex)\0"
    "on_drugsView_doubleClicked(QModelIndex)\0"
    "on_textButton_clicked()\0action\0"
    "historyAct_triggered(QAction*)\0"
};

const QMetaObject DrugsWidget::Internal::DrugSelector::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugSelector,
      qt_meta_data_DrugsWidget__Internal__DrugSelector, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugSelector::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugSelector))
        return static_cast<void*>(const_cast< DrugSelector*>(this));
    return QWidget::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: drugSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: drugSelected((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: retranslateUi((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: updateModelFilter(); break;
        case 4: on_InnView_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: on_drugsView_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 6: on_textButton_clicked(); break;
        case 7: historyAct_triggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void DrugsWidget::Internal::DrugSelector::drugSelected(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DrugsWidget::Internal::DrugSelector::drugSelected(const QModelIndex & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
