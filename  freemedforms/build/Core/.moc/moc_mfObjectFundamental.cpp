/****************************************************************************
** Meta object code from reading C++ file 'mfObjectFundamental.h'
**
** Created: Sun Sep 13 10:29:58 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/mfObjectFundamental.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfObjectFundamental.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_mfObjectFundamental[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,   20,   36,   20, 0x0a,
      63,   20,   36,   20, 0x0a,
      70,   20,   36,   20, 0x0a,
      78,   20,   20,   20, 0x0a,
     112,   20,   20,   20, 0x0a,
     158,  140,   20,   20, 0x0a,
     195,   20,   20,   20, 0x2a,
     233,  227,   20,   20, 0x0a,
     272,   20,  267,   20, 0x0a,
     303,   20,  291,   20, 0x0a,
     324,   20,  320,   20, 0x0a,
     356,   20,  346,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_mfObjectFundamental[] = {
    "mfObjectFundamental\0\0valueChanged()\0"
    "QString\0absoluteFileName()\0name()\0"
    "label()\0selectedValueChangedTo(QSet<int>)\0"
    "selectedValueChangedTo(int)\0"
    ",emitChangeSignal\0"
    "selectedValueChangedTo(QString,bool)\0"
    "selectedValueChangedTo(QString)\0value\0"
    "selectedValueChangedTo(QDateTime)\0"
    "bool\0isMultiSelection()\0QStringList\0"
    "selectedValues()\0int\0countSelectedValues()\0"
    "QSet<int>\0selectedValuesIds()\0"
};

const QMetaObject mfObjectFundamental::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_mfObjectFundamental,
      qt_meta_data_mfObjectFundamental, 0 }
};

const QMetaObject *mfObjectFundamental::metaObject() const
{
    return &staticMetaObject;
}

void *mfObjectFundamental::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mfObjectFundamental))
        return static_cast<void*>(const_cast< mfObjectFundamental*>(this));
    return QObject::qt_metacast(_clname);
}

int mfObjectFundamental::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged(); break;
        case 1: { QString _r = absoluteFileName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 2: { QString _r = name();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 3: { QString _r = label();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 4: selectedValueChangedTo((*reinterpret_cast< const QSet<int>(*)>(_a[1]))); break;
        case 5: selectedValueChangedTo((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 6: selectedValueChangedTo((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const bool(*)>(_a[2]))); break;
        case 7: selectedValueChangedTo((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: selectedValueChangedTo((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 9: { bool _r = isMultiSelection();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: { QStringList _r = selectedValues();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 11: { int _r = countSelectedValues();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 12: { QSet<int> _r = selectedValuesIds();
            if (_a[0]) *reinterpret_cast< QSet<int>*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void mfObjectFundamental::valueChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
