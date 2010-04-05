/****************************************************************************
** Meta object code from reading C++ file 'mfObject.h'
**
** Created: Sun Sep 13 10:29:57 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/mfObject.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mfObject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_mfObject[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   12, // methods
       9,  137, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,    9,   32,    9, 0x0a,
      47,    9,   32,    9, 0x0a,
      64,    9,   55,    9, 0x0a,
      84,    9,   72,    9, 0x0a,
     105,   98,   93,    9, 0x0a,
     128,    9,   32,    9, 0x0a,
     149,    9,  145,    9, 0x0a,
     164,    9,  154,    9, 0x0a,
     170,    9,   93,    9, 0x0a,
     184,    9,   93,    9, 0x0a,
     200,  196,    9,    9, 0x0a,
     222,  219,    9,    9, 0x0a,
     241,  196,    9,    9, 0x0a,
     268,  219,    9,    9, 0x0a,
     288,  284,    9,    9, 0x0a,
     306,  196,    9,    9, 0x0a,
     331,    9,    9,    9, 0x0a,
     365,  356,    9,    9, 0x0a,
     388,    9,   55,    9, 0x0a,
     414,  404,   32,    9, 0x0a,
     444,  404,   32,    9, 0x0a,
     472,    9,   32,    9, 0x0a,
     500,  491,   93,    9, 0x0a,
     517,    9,   32,    9, 0x0a,

 // properties: name, type, flags
     527,   32, 0x0a095001,
     532,   32, 0x0a095001,
     538,   32, 0x0a095103,
     553,   55, 0xff095103,
     219,   55, 0xff095103,
     559,   55, 0xff095103,
     571,   55, 0xff095103,
     581,   55, 0xff095103,
     284,  154, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_mfObject[] = {
    "mfObject\0\0formUiMustBeUpdated()\0QString\0"
    "name()\0label()\0QVariant\0value()\0"
    "QStringList\0values()\0bool\0values\0"
    "setValues(QStringList)\0interpretation()\0"
    "int\0id()\0QSet<int>\0ids()\0interpreted()\0"
    "completed()\0val\0setValue(QVariant)\0"
    "id\0setChildrenId(int)\0setInterpretation(QString)\0"
    "setId(QVariant)\0ids\0setIds(QSet<int>)\0"
    "setInterpreted(QVariant)\0"
    "allChildrenInterpreted()\0complete\0"
    "setCompleted(QVariant)\0sumOfChildren()\0"
    "separator\0sumOfInterpretValues(QString)\0"
    "sumOfCheckedLabels(QString)\0"
    "absoluteFileName()\0fileName\0"
    "addFile(QString)\0toDebug()\0name\0label\0"
    "interpretation\0value\0interpreted\0"
    "completed\0modified\0"
};

const QMetaObject mfObject::staticMetaObject = {
    { &mfObjectFundamental::staticMetaObject, qt_meta_stringdata_mfObject,
      qt_meta_data_mfObject, 0 }
};

const QMetaObject *mfObject::metaObject() const
{
    return &staticMetaObject;
}

void *mfObject::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_mfObject))
        return static_cast<void*>(const_cast< mfObject*>(this));
    return mfObjectFundamental::qt_metacast(_clname);
}

int mfObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = mfObjectFundamental::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: formUiMustBeUpdated(); break;
        case 1: { QString _r = name();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 2: { QString _r = label();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 3: { QVariant _r = value();
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 4: { QStringList _r = values();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = _r; }  break;
        case 5: { bool _r = setValues((*reinterpret_cast< const QStringList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { QString _r = interpretation();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 7: { int _r = id();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: { QSet<int> _r = ids();
            if (_a[0]) *reinterpret_cast< QSet<int>*>(_a[0]) = _r; }  break;
        case 9: { bool _r = interpreted();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: { bool _r = completed();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: setValue((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 12: setChildrenId((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 13: setInterpretation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: setId((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 15: setIds((*reinterpret_cast< const QSet<int>(*)>(_a[1]))); break;
        case 16: setInterpreted((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 17: allChildrenInterpreted(); break;
        case 18: setCompleted((*reinterpret_cast< const QVariant(*)>(_a[1]))); break;
        case 19: { QVariant _r = sumOfChildren();
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 20: { QString _r = sumOfInterpretValues((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 21: { QString _r = sumOfCheckedLabels((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 22: { QString _r = absoluteFileName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 23: { bool _r = addFile((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 24: { QString _r = toDebug();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 25;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = name(); break;
        case 1: *reinterpret_cast< QString*>(_v) = label(); break;
        case 2: *reinterpret_cast< QString*>(_v) = interpretation(); break;
        case 3: *reinterpret_cast< QVariant*>(_v) = value(); break;
        case 4: *reinterpret_cast< QVariant*>(_v) = id(); break;
        case 5: *reinterpret_cast< QVariant*>(_v) = interpreted(); break;
        case 6: *reinterpret_cast< QVariant*>(_v) = completed(); break;
        case 7: *reinterpret_cast< QVariant*>(_v) = isModified(); break;
        case 8: *reinterpret_cast< QSet<int>*>(_v) = ids(); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: setInterpretation(*reinterpret_cast< QString*>(_v)); break;
        case 3: setValue(*reinterpret_cast< QVariant*>(_v)); break;
        case 4: setId(*reinterpret_cast< QVariant*>(_v)); break;
        case 5: setInterpreted(*reinterpret_cast< QVariant*>(_v)); break;
        case 6: setCompleted(*reinterpret_cast< QVariant*>(_v)); break;
        case 7: setModified(*reinterpret_cast< QVariant*>(_v)); break;
        case 8: setIds(*reinterpret_cast< QSet<int>*>(_v)); break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void mfObject::formUiMustBeUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
