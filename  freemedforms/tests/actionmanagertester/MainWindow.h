#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include <tkContextManager.h>
#include <tkRichTextEditor.h>
#include <tkListView.h>

#include <tkMainWindow.h>


class TestWidget;

class MainWindow : public tkMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    void initialize();
    void createUi();

public Q_SLOTS:
    void openDialog();
    bool event(QEvent *e);

public:
    TestWidget *t1, *t2;
    QPushButton *but;


    QAction *aNew;
    QAction *aOpen;
    QAction *aSave;
    QAction *aSaveAs;
    QAction *aPrint;
    QAction *aClose;
    QAction *aQuit;

    QAction *aUndo;
    QAction *aRedo;
    QAction *aCut;
    QAction *aCopy;
    QAction *aPaste;

    tkRichTextEditor *text1;
    tkRichTextEditor *text2;
    tkRichTextEditor *text3;

    tkListView *view1;
};

#endif // MAINWINDOW_H
