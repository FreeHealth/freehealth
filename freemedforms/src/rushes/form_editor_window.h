#ifndef FORM_EDITOR_WINDOW_H
#define FORM_EDITOR_WINDOW_H

#include <QGridLayout>

#include "mfObject.h"

#include "ui_form_editor_window.h"

class FormEditorWindow : public QMainWindow, private Ui::FormEditorWindow
{
     Q_OBJECT

public:
     FormEditorWindow(QWidget *parent = 0);

private:
     mfObject *m_object;
     QGridLayout *m_gridLayoutForm;

     void init();
     void open();
     void save();
     void saveAs();

private slots:
     void on_actionOpen_triggered() { open(); }
     void on_actionSave_triggered() { save(); }
     void on_actionSaveAs_triggered() { saveAs(); }
};

#endif
