#include <QLabel>

#include "mfWidgetSelector.h"

mfWidgetSelector::mfWidgetSelector(QWidget *parent)
     : QWidget(parent),
       m_widget(0)
{
     setupUi(this);

     init();
}

void mfWidgetSelector::init()
{
     m_baseLayout = new QVBoxLayout(frameBase);

     comboBoxType->clear();

     foreach (const QString &typeName, mfObjectFundamental::typeNames)
          comboBoxType->addItem(typeName);
}

void mfWidgetSelector::on_comboBoxType_currentIndexChanged(int index)
{
     if (!index)
     {
          if (m_widget)
          {
               delete m_widget;
               m_widget = 0;
          } else
               return;

          return;
     }

     if (m_widget) // already a widget
     {
          if (m_widget->object()->type() == index)
               return; // the same => go out

          delete m_widget->object();
          delete m_widget;
     }

     // create the new widget
     mfObject *object = new mfObject;
     object->setType(index);
     m_widget = new mfWidget(object);
     m_baseLayout->addWidget(m_widget);
}
