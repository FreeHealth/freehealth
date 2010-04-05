/****************************************************************************
** Meta object code from reading C++ file 'drugswidgetfactory.h'
**
** Created: Mon Feb 1 19:13:12 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/drugsplugin/drugswidgetfactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'drugswidgetfactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DrugsWidget__Internal__DrugsWidgetsFactory[] = {

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

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugsWidgetsFactory[] = {
    "DrugsWidget::Internal::DrugsWidgetsFactory\0"
};

const QMetaObject DrugsWidget::Internal::DrugsWidgetsFactory::staticMetaObject = {
    { &Core::IFormWidgetFactory::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugsWidgetsFactory,
      qt_meta_data_DrugsWidget__Internal__DrugsWidgetsFactory, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugsWidgetsFactory::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugsWidgetsFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugsWidgetsFactory))
        return static_cast<void*>(const_cast< DrugsWidgetsFactory*>(this));
    typedef Core::IFormWidgetFactory QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugsWidgetsFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IFormWidgetFactory QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_DrugsWidget__Internal__DrugsPrescriptorWidget[] = {

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

static const char qt_meta_stringdata_DrugsWidget__Internal__DrugsPrescriptorWidget[] = {
    "DrugsWidget::Internal::DrugsPrescriptorWidget\0"
};

const QMetaObject DrugsWidget::Internal::DrugsPrescriptorWidget::staticMetaObject = {
    { &Core::IFormWidget::staticMetaObject, qt_meta_stringdata_DrugsWidget__Internal__DrugsPrescriptorWidget,
      qt_meta_data_DrugsWidget__Internal__DrugsPrescriptorWidget, 0 }
};

const QMetaObject *DrugsWidget::Internal::DrugsPrescriptorWidget::metaObject() const
{
    return &staticMetaObject;
}

void *DrugsWidget::Internal::DrugsPrescriptorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DrugsWidget__Internal__DrugsPrescriptorWidget))
        return static_cast<void*>(const_cast< DrugsPrescriptorWidget*>(this));
    typedef Core::IFormWidget QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int DrugsWidget::Internal::DrugsPrescriptorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IFormWidget QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
