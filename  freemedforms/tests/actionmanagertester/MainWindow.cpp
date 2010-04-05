#include "MainWindow.h"
#include "TestWidget.h"


#include <tkActionManager.h>

#include <tkConstants.h>
#include <tkContextManager.h>
#include <tkTheme.h>
#include <tkConstantTranslations.h>

MainWindow::MainWindow(QWidget *parent) : tkMainWindow(parent)
{
    setObjectName("MainWindow");
}

void MainWindow::initialize()
{
    createFileMenu();
    createFileActions();
    createEditMenu();
    createEditActions();
    createFormatMenu();
    createConfigurationMenu();
    createUi();

    QTranslator t(this);
    if (!t.load("../global_resources/translations/qt_fr.qm"))
        qWarning() << "no trans";
    qApp->installTranslator(&t);

    tkActionManager::instance()->retranslateMenusAndActions();
    tkContextManager::instance()->updateContext();
}

void MainWindow::createUi()
{
    QWidget *w = new QWidget(this);
    QGridLayout *l = new QGridLayout(w);
    t1 = new TestWidget(this);
    t2 = new TestWidget(this);
    but = new QPushButton(this);
    text1 = new tkRichTextEditor(this);
    text2 = new tkRichTextEditor(this, tkRichTextEditor::WithTables);
    text3 = new tkRichTextEditor(this, tkRichTextEditor::WithTables | tkRichTextEditor::WithIO);
    but->setText("reerte");
    but->setIcon(tkTheme::instance()->icon("filenew.png"));

    QStringListModel *m = new QStringListModel(this);
    m->setStringList( QStringList() << "un" << "deux" );
    view1 = new tkListView(this, tkListView::AllActions);
    view1->setModel(m);
    view1->setObjectName("view1");
    view1->useContextMenu(false);

    l->addWidget(text1);
    l->addWidget(text2);
    l->addWidget(text3);
    l->addWidget(t1);
    l->addWidget(t2);
    l->addWidget(view1);
    l->addWidget(but);
    setCentralWidget(w);

    t1->init();
    t2->init();
}

void MainWindow::openDialog()
{
    QDialog dlg;
    QGridLayout l(&dlg);
    QTextEdit t(&dlg);
    l.addWidget(&t, 0,0);
    t.setText( objectName() + " creates the dialog . Sender = " + sender()->objectName() );
    dlg.exec();
}

bool MainWindow::event(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        qWarning() << "translate me";
        tkActionManager::instance()->retranslateMenusAndActions();
        e->accept();
        return true;
    }
    return false;
}

