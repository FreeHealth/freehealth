/****************************************************************************
** Meta object code from reading C++ file 'girwidget.h'
**
** Created: Mon Feb 1 19:09:59 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/aggirplugin/girwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'girwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Gir__Internal__GirWidgetFactory[] = {

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

static const char qt_meta_stringdata_Gir__Internal__GirWidgetFactory[] = {
    "Gir::Internal::GirWidgetFactory\0"
};

const QMetaObject Gir::Internal::GirWidgetFactory::staticMetaObject = {
    { &Core::IFormWidgetFactory::staticMetaObject, qt_meta_stringdata_Gir__Internal__GirWidgetFactory,
      qt_meta_data_Gir__Internal__GirWidgetFactory, 0 }
};

const QMetaObject *Gir::Internal::GirWidgetFactory::metaObject() const
{
    return &staticMetaObject;
}

void *Gir::Internal::GirWidgetFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Gir__Internal__GirWidgetFactory))
        return static_cast<void*>(const_cast< GirWidgetFactory*>(this));
    typedef Core::IFormWidgetFactory QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Gir::Internal::GirWidgetFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IFormWidgetFactory QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Gir__Internal__GirUi[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      28,   22,   21,   21, 0x0a,
      62,   21,   21,   21, 0x0a,
      83,   21,   21,   21, 0x0a,
     104,   21,   21,   21, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Gir__Internal__GirUi[] = {
    "Gir::Internal::GirUi\0\0radio\0"
    "updateGirString(QAbstractButton*)\0"
    "on_aButton_clicked()\0on_bButton_clicked()\0"
    "on_cButton_clicked()\0"
};

const QMetaObject Gir::Internal::GirUi::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Gir__Internal__GirUi,
      qt_meta_data_Gir__Internal__GirUi, 0 }
};

const QMetaObject *Gir::Internal::GirUi::metaObject() const
{
    return &staticMetaObject;
}

void *Gir::Internal::GirUi::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Gir__Internal__GirUi))
        return static_cast<void*>(const_cast< GirUi*>(this));
    return QWidget::qt_metacast(_clname);
}

int Gir::Internal::GirUi::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateGirString((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 1: on_aButton_clicked(); break;
        case 2: on_bButton_clicked(); break;
        case 3: on_cButton_clicked(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_Gir__Internal__GirWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Gir__Internal__GirWidget[] = {
    "Gir::Internal::GirWidget\0\0retranslate()\0"
};

const QMetaObject Gir::Internal::GirWidget::staticMetaObject = {
    { &Core::IFormWidget::staticMetaObject, qt_meta_stringdata_Gir__Internal__GirWidget,
      qt_meta_data_Gir__Internal__GirWidget, 0 }
};

const QMetaObject *Gir::Internal::GirWidget::metaObject() const
{
    return &staticMetaObject;
}

void *Gir::Internal::GirWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Gir__Internal__GirWidget))
        return static_cast<void*>(const_cast< GirWidget*>(this));
    typedef Core::IFormWidget QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Gir::Internal::GirWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IFormWidget QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
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
