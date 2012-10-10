#ifndef FREETEXT_H
#define FREETEXT_H

#include "ui_freetextdialog.h"
#include <QDialog>
#include <QString>

class FreeText:public QDialog, public Ui::FreeTextDialog
{
    Q_OBJECT
    public:
        FreeText(QWidget * parent = 0);
        ~FreeText();
        QString getFreeText();
};

#endif
