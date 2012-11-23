#ifndef TESTCLASS_H
#define TESTCLASS_H

#include <QWidget>
#include <QMainWindow>

class testClass : public QWidget
{
    Q_OBJECT
public:
    testClass( QMainWindow *parent = 0 );
    ~testClass();

public Q_SLOTS:
    void on_action_triggered();
    void on_menuEdit_aboutToShow();
};

#endif // TESTCLASS_H
