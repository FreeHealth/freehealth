/****************************************************************************
** Meta object code from reading C++ file 'iformwidgetfactory.h'
**
** Created: Sun Sep 13 10:29:54 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/iformwidgetfactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'iformwidgetfactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__IFormWidgetFactory[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_Core__IFormWidgetFactory[] = {
    "Core::IFormWidgetFactory\0"
};

const QMetaObject Core::IFormWidgetFactory::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Core__IFormWidgetFactory,
      qt_meta_data_Core__IFormWidgetFactory, 0 }
};

const QMetaObject *Core::IFormWidgetFactory::metaObject() const
{
    return &staticMetaObject;
}

void *Core::IFormWidgetFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__IFormWidgetFactory))
        return static_cast<void*>(const_cast< IFormWidgetFactory*>(this));
    return QObject::qt_metacast(_clname);
}

int Core::IFormWidgetFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Core__IFormWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Core__IFormWidget[] = {
    "Core::IFormWidget\0\0retranslate()\0"
};

const QMetaObject Core::IFormWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Core__IFormWidget,
      qt_meta_data_Core__IFormWidget, 0 }
};

const QMetaObject *Core::IFormWidget::metaObject() const
{
    return &staticMetaObject;
}

void *Core::IFormWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__IFormWidget))
        return static_cast<void*>(const_cast< IFormWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Core::IFormWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: retranslate(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
