/****************************************************************************
** Meta object code from reading C++ file 'druginfo.h'
**
** Created: Mon Feb 1 19:12:59 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugswidget/druginfo.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'druginfo.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DrugInfo[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x09,
      42,   32,   32,   32, 0x09,
      51,   32,   32,   32, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugInfo[] = {
    "DrugsWidget::Internal::DrugInfo\0\0"
    "accept()\0reject()\0done()\0"
};

const QMetaObject DrugsWidget::Internal::DrugInfo::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugInfo,
      qt_meta_data_DrugsWidget__Internal__DrugInfo, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugInfo::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugInfo))
        return static_cast<void*>(const_cast< DrugInfo*>(this));
    return QDialog::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accept(); break;
        case 1: reject(); break;
        case 2: done(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
