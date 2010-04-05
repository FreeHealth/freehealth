#ifndef MFMAINCENTRALWIDGET_H
#define MFMAINCENTRALWIDGET_H

#include "ui_mfMainCentralWidget.h"

class mfMainCentralWidget : public QWidget, private Ui::mfMainCentralWidget {
    Q_OBJECT
public:
    mfMainCentralWidget(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
};

#endif // MFMAINCENTRALWIDGET_H
