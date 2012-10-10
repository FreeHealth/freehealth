#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QtGui>

#include <tkContext.h>


class TestWidget : public QWidget
{
    Q_OBJECT
public:
    TestWidget(QWidget *parent=0);

    void init();

public Q_SLOTS:
    void openDialog();

public:
    QAction *m_OpenDialog;
    QLineEdit *line;
};


class TestWidgetContext : public tkContext
{
public:
    TestWidgetContext(TestWidget *w, const QList<int> &context) :
            tkContext(w), m_Widget(w), m_Context(context) {}

    QList<int> context() const { return m_Context; }
    QWidget *widget()  { return m_Widget; }

private:
    TestWidget *m_Widget;
    QList<int> m_Context;
};

#endif // TESTWIDGET_H
