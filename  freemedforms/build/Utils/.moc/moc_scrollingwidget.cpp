/****************************************************************************
** Meta object code from reading C++ file 'scrollingwidget.h'
**
** Created: Sun Sep 13 10:27:52 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../libs/utils/widgets/scrollingwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scrollingwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Utils__ScrollingWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   12, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // properties: name, type, flags
      31,   23, 0x0a095103,
      40,   36, 0x02095103,

       0        // eod
};

static const char qt_meta_stringdata_Utils__ScrollingWidget[] = {
    "Utils::ScrollingWidget\0QString\0text\0"
    "int\0timerDelay\0"
};

const QMetaObject Utils::ScrollingWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Utils__ScrollingWidget,
      qt_meta_data_Utils__ScrollingWidget, 0 }
};

const QMetaObject *Utils::ScrollingWidget::metaObject() const
{
    return &staticMetaObject;
}

void *Utils::ScrollingWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Utils__ScrollingWidget))
        return static_cast<void*>(const_cast< ScrollingWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Utils::ScrollingWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        case 1: *reinterpret_cast< int*>(_v) = timerDelay(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setText(*reinterpret_cast< QString*>(_v)); break;
        case 1: setTimerDelay(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
