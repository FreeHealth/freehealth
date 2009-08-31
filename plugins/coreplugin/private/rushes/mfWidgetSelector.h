#ifndef MFWIDGET_SELECTOR_H
#define MFWIDGET_SELECTOR_H

#include <QComboBox>

#include "mfObject.h"
#include "mfWidget.h"

#include "ui_mfWidgetSelector.h"

class mfWidgetSelector : public QWidget, private Ui::mfWidgetSelector
{
     Q_OBJECT

public:
     mfWidgetSelector(QWidget *parent = 0);

private slots:
     void on_comboBoxType_currentIndexChanged(int index);

private:
     QVBoxLayout *m_baseLayout;
     mfWidget *m_widget;

     void init();
};

#endif
