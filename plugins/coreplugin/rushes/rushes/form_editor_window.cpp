#include <QFileDialog>

#include "mfWidgetSelector.h"

#include "form_editor_window.h"

FormEditorWindow::FormEditorWindow(QWidget *parent)
     : QMainWindow(parent),
       m_object(0)
{
     setupUi(this);

     setAttribute(Qt::WA_DeleteOnClose, true);

     init();
}

void FormEditorWindow::init()
{
     QWidget *widget = new QWidget;
     scrollAreaForm->setWidget(widget);
     m_gridLayoutForm = new QGridLayout(widget);

     mfWidgetSelector *widgetSelector = new mfWidgetSelector;
     m_gridLayoutForm->addWidget(widgetSelector, 0, 0);
     widgetSelector = new mfWidgetSelector;
     m_gridLayoutForm->addWidget(widgetSelector, 0, 1);
     widgetSelector = new mfWidgetSelector;
     m_gridLayoutForm->addWidget(widgetSelector, 1, 0);
     widgetSelector = new mfWidgetSelector;
     m_gridLayoutForm->addWidget(widgetSelector, 1, 1);
}

void FormEditorWindow::open()
{
     QString fileName = QFileDialog::getOpenFileName(this, tr("Open a form"));
}

void FormEditorWindow::save()
{

}

void FormEditorWindow::saveAs()
{
     QString fileName = QFileDialog::getSaveFileName(this, tr("Save a form as..."));
}
