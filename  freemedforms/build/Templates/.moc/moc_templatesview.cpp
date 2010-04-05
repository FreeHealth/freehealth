/****************************************************************************
** Meta object code from reading C++ file 'templatesview.h'
**
** Created: Mon Feb 1 19:10:32 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/templatesplugin/templatesview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'templatesview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Templates__TemplatesView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   25,   25,   25, 0x09,
      61,   25,   25,   25, 0x09,
      74,   25,   25,   25, 0x09,
      92,   25,   25,   25, 0x09,
     110,  104,   25,   25, 0x09,
     121,   25,   25,   25, 0x29,
     133,   25,  128,   25, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_Templates__TemplatesView[] = {
    "Templates::TemplatesView\0\0"
    "currentItemChanged()\0addCategory()\0"
    "removeItem()\0editCurrentItem()\0"
    "saveModel()\0state\0lock(bool)\0lock()\0"
    "bool\0printTemplate()\0"
};

const QMetaObject Templates::TemplatesView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Templates__TemplatesView,
      qt_meta_data_Templates__TemplatesView, 0 }
};

const QMetaObject *Templates::TemplatesView::metaObject() const
{
    return &staticMetaObject;
}

void *Templates::TemplatesView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Templates__TemplatesView))
        return static_cast<void*>(const_cast< TemplatesView*>(this));
    return QWidget::qt_metacast(_clname);
}

int Templates::TemplatesView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: currentItemChanged(); break;
        case 1: addCategory(); break;
        case 2: removeItem(); break;
        case 3: editCurrentItem(); break;
        case 4: saveModel(); break;
        case 5: lock((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: lock(); break;
        case 7: { bool _r = printTemplate();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Templates::TemplatesView::currentItemChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
