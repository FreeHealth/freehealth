/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Sun Sep 13 20:55:45 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/fmfcoreplugin/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Core__MainWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   17,   18,   17, 0x0a,
      34,   17,   18,   17, 0x0a,
      59,   17,   18,   17, 0x0a,
      69,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Core__MainWindow[] = {
    "Core::MainWindow\0\0bool\0openFile()\0"
    "applicationPreferences()\0aboutQt()\0"
    "aboutPlugins()\0"
};

const QMetaObject Core::MainWindow::staticMetaObject = {
    { &Core::IMainWindow::staticMetaObject, qt_meta_stringdata_Core__MainWindow,
      qt_meta_data_Core__MainWindow, 0 }
};

const QMetaObject *Core::MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *Core::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Core__MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    typedef Core::IMainWindow QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Core::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IMainWindow QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = openFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { bool _r = applicationPreferences();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = aboutQt();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: aboutPlugins(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
