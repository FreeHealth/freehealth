/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon Feb 1 19:09:28 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../plugins/fmfmainwindowplugin/mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWin__MainWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      26,   20,   21,   20, 0x0a,
      53,   37,   21,   20, 0x0a,
     102,   93,   21,   20, 0x2a,
     120,   20,   21,   20, 0x0a,
     145,   20,   20,   20, 0x0a,
     164,   20,   20,   20, 0x0a,
     185,   20,   20,   20, 0x0a,
     210,   20,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWin__MainWindow[] = {
    "MainWin::MainWindow\0\0bool\0openFile()\0"
    "filename,iolist\0loadFile(QString,QList<Core::IFormIO*>)\0"
    "filename\0loadFile(QString)\0"
    "applicationPreferences()\0updateCheckerEnd()\0"
    "openLastOpenedForm()\0aboutToShowRecentFiles()\0"
    "openRecentFile()\0"
};

const QMetaObject MainWin::MainWindow::staticMetaObject = {
    { &Core::IMainWindow::staticMetaObject, qt_meta_stringdata_MainWin__MainWindow,
      qt_meta_data_MainWin__MainWindow, 0 }
};

const QMetaObject *MainWin::MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWin::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWin__MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    typedef Core::IMainWindow QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int MainWin::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Core::IMainWindow QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = openFile();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { bool _r = loadFile((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QList<Core::IFormIO*>(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = loadFile((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = applicationPreferences();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: updateCheckerEnd(); break;
        case 5: openLastOpenedForm(); break;
        case 6: aboutToShowRecentFiles(); break;
        case 7: openRecentFile(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
