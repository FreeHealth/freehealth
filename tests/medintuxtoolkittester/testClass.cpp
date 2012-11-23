#include "testClass.h"
#include <tkActionManager.h>
#include <tkRichTextEditor.h>
#include <tkStringListModel.h>
#include <tkStringListView.h>
#include <tkListView.h>

#include <QDebug>
#include <QtGui>

using namespace tkConstants;

testClass::testClass( QMainWindow *parent ) :
        QWidget(parent)
{
    tkActionManager *m = tkActionManager::instance();
    m->createDefaultMenusAndGroups(this);
    m->createDefaultActions(this);
//    m->createMenu( "myMenu", M_EDIT, "text", "icon.png", this);
//    m->createAction( "action", G_EDIT_OTHER, this);
//    m->setActionDatas( "action", "Automatic slot connection" );

//    foreach( QObject *o, qFindChildren<QObject*>(this))
//        qWarning() << o->objectName();

    QMetaObject::connectSlotsByName(this);


    QGridLayout *l = new QGridLayout(this);
    tkRichTextEditor *te1 = new tkRichTextEditor(this,tkRichTextEditor::Full|tkRichTextEditor::WithLoad|tkRichTextEditor::WithSave|tkRichTextEditor::WithTables);
    tkRichTextEditor *te2 = new tkRichTextEditor(this);
    l->addWidget(te1);
    l->addWidget(te2);
    m->refreshAll();


    tkStringListModel *model = new tkStringListModel(qApp);
    model->setStringList( QStringList() << "1" << "2" << "3" << "4" );
    model->setStringEditable(true);

    tkStringListView *tklv = new tkStringListView( this );
    tklv->setButtons( tkListView::AllButtons );
    tklv->setActions( tkListView::AllActions );
    tklv->setModel( model );
    tklv->setEditTriggers(QAbstractItemView::AllEditTriggers);

    QStringListModel *model2 = new QStringListModel(qApp);
    model2->setStringList( QStringList() << "5" << "6" << "7" << "8" );
    tkStringListView *tklv2= new tkStringListView( this );
    tklv2->setButtons( tkListView::AllButtons );
    tklv2->setActions( tkListView::AllActions );
    tklv2->setModel( model2 );
    tklv2->setEditTriggers(QAbstractItemView::AllEditTriggers);

    tklv2->setObjectName("tklv2");
    tklv->setObjectName("tklv");

    l->addWidget(tklv);
    l->addWidget(tklv2);

    parent->setMenuBar( m->menubar( MENUBAR ) );

}

testClass::~testClass()
{
    qWarning() << "~testClass";
}

void testClass::on_action_triggered()
{
    qWarning() << "SLOOOOT";
}

void testClass::on_menuEdit_aboutToShow()
{
    qWarning() << "aboutToShow";
}
