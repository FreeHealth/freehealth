/****************************************************************************
** Meta object code from reading C++ file 'tableeditor.h'
**
** Created: Mon Feb 1 11:53:34 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/texteditorplugin/tableeditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tableeditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Editor__TableEditor[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x0a,
      32,   20,   20,   20, 0x0a,
      50,   20,   20,   20, 0x0a,
      64,   20,   20,   20, 0x0a,
      78,   20,   20,   20, 0x0a,
      95,   20,   20,   20, 0x0a,
     112,   20,   20,   20, 0x0a,
     130,   20,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Editor__TableEditor[] = {
    "Editor::TableEditor\0\0addTable()\0"
    "tableProperties()\0tableAddRow()\0"
    "tableAddCol()\0tableRemoveRow()\0"
    "tableRemoveCol()\0tableMergeCells()\0"
    "tableSplitCells()\0"
};

const QMetaObject Editor::TableEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Editor__TableEditor,
      qt_meta_data_Editor__TableEditor, 0 }
};

const QMetaObject *Editor::TableEditor::metaObject() const
{
    return &staticMetaObject;
}

void *Editor::TableEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Editor__TableEditor))
        return static_cast<void*>(const_cast< TableEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int Editor::TableEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addTable(); break;
        case 1: tableProperties(); break;
        case 2: tableAddRow(); break;
        case 3: tableAddCol(); break;
        case 4: tableRemoveRow(); break;
        case 5: tableRemoveCol(); break;
        case 6: tableMergeCells(); break;
        case 7: tableSplitCells(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
