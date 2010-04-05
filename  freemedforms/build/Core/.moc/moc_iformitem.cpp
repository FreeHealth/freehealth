/****************************************************************************
** Meta object code from reading C++ file 'iformitem.h'
**
** Created: Tue Feb 9 09:15:44 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/coreplugin/iformitem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'iformitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__FormItem[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Core__FormItem[] = {
    "Core::FormItem\0\0languageChanged()\0"
};

const QMetaObject Core::FormItem::staticMetaObject = {
    { &Core::FormItemIdentifiants::staticMetaObject, qt_meta_stringdata_Core__FormItem,
      qt_meta_data_Core__FormItem, 0 }
};

const QMetaObject *Core::FormItem::metaObject() const
{
    return &staticMetaObject;
}

void *Core::FormItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__FormItem))
        return static_cast<void*>(const_cast< FormItem*>(this));
    typedef Core::FormItemIdentifiants QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Core::FormItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::FormItemIdentifiants QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: languageChanged(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_Core__FormPage[] = {

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

static const char qt_meta_stringdata_Core__FormPage[] = {
    "Core::FormPage\0"
};

const QMetaObject Core::FormPage::staticMetaObject = {
    { &FormItem::staticMetaObject, qt_meta_stringdata_Core__FormPage,
      qt_meta_data_Core__FormPage, 0 }
};

const QMetaObject *Core::FormPage::metaObject() const
{
    return &staticMetaObject;
}

void *Core::FormPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__FormPage))
        return static_cast<void*>(const_cast< FormPage*>(this));
    return FormItem::qt_metacast(_clname);
}

int Core::FormPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = FormItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Core__FormMain[] = {

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

static const char qt_meta_stringdata_Core__FormMain[] = {
    "Core::FormMain\0"
};

const QMetaObject Core::FormMain::staticMetaObject = {
    { &FormItem::staticMetaObject, qt_meta_stringdata_Core__FormMain,
      qt_meta_data_Core__FormMain, 0 }
};

const QMetaObject *Core::FormMain::metaObject() const
{
    return &staticMetaObject;
}

void *Core::FormMain::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__FormMain))
        return static_cast<void*>(const_cast< FormMain*>(this));
    return FormItem::qt_metacast(_clname);
}

int Core::FormMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = FormItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
