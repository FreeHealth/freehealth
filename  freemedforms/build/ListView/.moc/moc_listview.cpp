/****************************************************************************
** Meta object code from reading C++ file 'listview.h'
**
** Created: Mon Feb 1 11:54:05 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/listviewplugin/listview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'listview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Views__ListView[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      24,   16,   17,   16, 0x0a,
      41,   16,   16,   16, 0x0a,
      61,   16,   16,   16, 0x0a,
      71,   16,   16,   16, 0x0a,
      84,   16,   16,   16, 0x0a,
      95,   16,   16,   16, 0x0a,
     104,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Views__ListView[] = {
    "Views::ListView\0\0QMenu*\0getContextMenu()\0"
    "contextMenu(QPoint)\0addItem()\0"
    "removeItem()\0moveDown()\0moveUp()\0"
    "on_edit_triggered()\0"
};

const QMetaObject Views::ListView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Views__ListView,
      qt_meta_data_Views__ListView, 0 }
};

const QMetaObject *Views::ListView::metaObject() const
{
    return &staticMetaObject;
}

void *Views::ListView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Views__ListView))
        return static_cast<void*>(const_cast< ListView*>(this));
    return QWidget::qt_metacast(_clname);
}

int Views::ListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QMenu* _r = getContextMenu();
            if (_a[0]) *reinterpret_cast< QMenu**>(_a[0]) = _r; }  break;
        case 1: contextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: addItem(); break;
        case 3: removeItem(); break;
        case 4: moveDown(); break;
        case 5: moveUp(); break;
        case 6: on_edit_triggered(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
