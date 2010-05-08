/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "formmanager.h"
#include "iformitem.h"
#include "iformio.h"
#include "formplaceholder.h"

#include <formmanagerplugin/iformwidgetfactory.h>

#include <coreplugin/icore.h>
#include <coreplugin/uniqueidmanager.h>
#include <coreplugin/modemanager/modemanager.h>

#include <utils/global.h>

#include <extensionsystem/pluginmanager.h>

#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QStackedLayout>
#include <QSplitter>
#include <QWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPointer>
#include <QMap>
#include <QApplication>


using namespace Form;
using namespace Form::Internal;

static inline ExtensionSystem::PluginManager *pluginManager() { return ExtensionSystem::PluginManager::instance(); }
static inline Core::ModeManager *modeManager() { return Core::ICore::instance()->modeManager(); }


namespace Form {
namespace Internal {
class FormManagerPrivate
{
public:
    FormManagerPrivate(FormManager *parent) :
            m_Holder(0), q(parent)
    {}

    ~FormManagerPrivate()
    {
        // DO NOT DELETE m_UuidManager
        if (m_Holder) {
            delete m_Holder;
            m_Holder = 0;
        }
        /** \todo Delete FormItem ?? */
    }

    QTreeWidget *formsTreeWidget(QTreeWidget *tree) const
    {
        Q_ASSERT(tree);
        int i = 0;
        QHash<FormMain *, QTreeWidgetItem *> items;
        foreach(FormMain *form, q->forms()) {
            QTreeWidgetItem *item = 0;
            if (items.keys().contains(form->formParent()))
                item =  new QTreeWidgetItem(items.value(form->formParent()), QStringList() << form->spec()->label());
            else
                item =  new QTreeWidgetItem(tree, QStringList() << form->spec()->label());
            items.insert(form, item);
            item->setData(0,Qt::UserRole,i);
            if (form->formParent())
                qWarning() << "Parent" << form->formParent()->spec()->label();
            ++i;
        }
        tree->resizeColumnToContents(0);
        tree->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        tree->header()->hide();
        return tree;
    }

    QStackedLayout *formsStackedLayout(QStackedLayout *stack) const
    {
        Q_ASSERT(stack);
        foreach(FormMain *form, q->forms()) {
            if (form->formWidget()) {
                QWidget *w = new QWidget;
                QVBoxLayout *vl = new QVBoxLayout(w);
                vl->addWidget(form->formWidget());
                stack->addWidget(w);
            }
        }
        return stack;
    }

public:
    QPointer<FormPlaceHolder> m_Holder;
    Core::UniqueIDManager *m_UuidManager;
    QMap<int, Form::FormMain *> m_MappedForms;

private:
    FormManager *q;
};
} // End Internal
} // End Form


FormManager *FormManager::m_Instance = 0;

FormManager *FormManager::instance()
{
    if (!m_Instance) {
        m_Instance = new FormManager(qApp);
    }
    return m_Instance;
}


FormManager::FormManager(QObject *parent)
        : QObject(parent), d(new Form::Internal::FormManagerPrivate(this))
{
    setObjectName("Form::FormManager");
    /** \todo Need to modify UID code to create a new private uid */
    d->m_UuidManager = Core::ICore::instance()->uniqueIDManager();
    d->m_Holder = new FormPlaceHolder;
}

FormManager::~FormManager()
{
    if (d) {
        delete d;
        d = 0;
    }
}

void FormManager::activateMode()
{
    modeManager()->activateMode(Core::Constants::MODE_PATIENT_FILE);
}

Core::UniqueIDManager *FormManager::uuidManager() const
{
    return d->m_UuidManager;
}

FormMain *FormManager::createForm(const QString &uuid, FormMain *parent)
{
    qWarning() << "FormManager Creating Form" << uuid;
    FormMain *f = 0;
    if (parent)
        f = new FormMain(parent);
    else
        f = new FormMain(this);
    if (!uuid.isEmpty())
        f->setUuid(uuid);
    d->m_MappedForms.insert(d->m_UuidManager->uniqueIdentifier(f->uuid()), f);
    return f;
}

QList<FormMain*> FormManager::forms() const
{
    return d->m_MappedForms.values();
}

bool FormManager::hasForm(const QString &uuid) const
{
    if (!d->m_UuidManager->hasUniqueIdentifier(uuid))
        return false;
    return true;
}

FormMain *FormManager::form(const QString &uuid) const
{
    if (!hasForm(uuid))
        return 0;
    int id = d->m_UuidManager->uniqueIdentifier(uuid);
    return d->m_MappedForms.value(id, 0);
}

bool FormManager::loadFile(const QString &filename, const QList<Form::IFormIO *> &iolist)
{
    if (filename.isEmpty())
        return false;

    // get all form readers (IFormIO)
    Form::IFormIO *reader = 0;
    QList<Form::IFormIO *> list;
    if (iolist.isEmpty())
         list = pluginManager()->getObjects<Form::IFormIO>();
    else
        list = iolist;

    // try to read form
    foreach(Form::IFormIO *io, list) {
        if (io->setFileName(filename) && io->canReadFile()) {
            if (io->loadForm())
                reader = io;
        }
    }
    if (!reader)
        return false;

    // populate FormPlaceHolder with new values
    if (d->m_Holder) {
//        d->m_Holder->formTree()->clear();
        d->m_Holder->clearFormStackLayout();
    }

//    d->formsTreeWidget(d->m_Holder->formTree());
    d->formsStackedLayout(d->m_Holder->formStackLayout());

    return true;
}


FormPlaceHolder *FormManager::formPlaceHolder() const
{
    return d->m_Holder;
}


bool FormManager::setFormObjects(QObject *root)
{
    // cast root
//    mfObject *rootObject = static_cast<mfObject*>(root);
//    if (!root)
//        return false;
//
//    // First object must be a form
//    if (rootObject->type() != mfObject::Type_Form) {
//        Utils::warningMessageBox(
//                tr("An error occured while creating the form."),
//                tr("The first object is not a <form> type."));
//        return false;
//    }
//
//    qWarning() << "FormManager::setObjects";

//    foreach(mfObject *o, rootObject->mfChildren()) {
//        warn(o);
//    }

//    m_Stack->removeWidget( m_Stack->widget( 0 ) );
//    delete m_Stack->widget( 0 );
//
//    // Retrieve Form Widget plugins
//    m_widgetPlugins = mfCore::pluginsManager()->plugins<mfFormWidgetInterface*>();
//    if ( m_widgetPlugins.count() < 2 )
//        tkLog::addError( this, tr( "Warning: missing Widget Plugins." ) );
//
//    // Create forms and populate treewidget
//    createForm( mfo );
//    m_Tree->resizeColumnToContents( LabelColumn );
//    m_Tree->setContextMenuPolicy( Qt::CustomContextMenu );
//
//    createActions();
//    createToolBar();
//    retranslateUi();
//
//    // Connect tree with stack
//    connect( m_Tree, SIGNAL( currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem * ) ),
//             this,   SLOT  ( treeToStack( QTreeWidgetItem * ) ) );
//    // Connect tree to popupmenu
//    connect( m_Tree, SIGNAL( customContextMenuRequested ( const QPoint & ) ),
//             this,   SLOT  ( createTreePopup( const QPoint & ) ) );
//
//    // Set the focus on the first tree item
//    if ( m_Tree->topLevelItemCount() )
//    {
//        m_Tree->setCurrentItem( m_Tree->topLevelItem( 0 ) );
//        m_FormNavList << m_Tree->topLevelItem( 0 );
//        m_FormNavOffset++;
//    }
//
//    // Run the onLoadScript
//    mfo->runOnLoadScript();
    return true;
}

bool FormManager::translateForms()
{
    // Translate the tree
//    translateTreeItem( m_Tree->topLevelItem( 0 ) );
//    m_Tree->resizeColumnToContents( LabelColumn );
//    retranslateUi();
    return true;
}

//void mfMainWidget::addNewFile()
//{
//    QTreeWidgetItem * item = m_Tree->currentItem();
//    if ( !item ) return;
//
//    // Append retrieved form to the actual form inside the good object
//    mfObject * parent = m_FormList.at( item->data( LabelColumn, Qt::UserRole ).toInt() );
//    if ( !parent ) return;
//
//    // Ask for a file and retreive the form using mfFormsIO
//    mfIOInterface *ioPlugin = mfCore::pluginsManager()->currentIO();
//    if ( !ioPlugin )
//    {
//        tkLog::addError( this , tr( "Main Wigdet ERROR : Laoding form : no IO plugin defined." ) );
//        return;
//    }
//
//    // Ask a file
//    QString fileName = QFileDialog::getOpenFileName( this, qApp->applicationName() , mfCore::settings()->formPath() );
//    if ( fileName.isEmpty() ) return;
//
//    mfObject * mfo = ioPlugin->loadForm( mfIOPlace::fromLocalFile( fileName ) );
//    if ( !mfo ) return;
//
//    mfo->setParent( parent );
//    createForm( mfo, item );
//    m_Tree->resizeColumnToContents( LabelColumn );
//    tkLog::addMessage( this , tr ( "Main Widget INFO : Form %1 correctly added" ).arg( fileName ) );
//}


//void mfMainWidget::removeSubForm()
//{
//    QTreeWidgetItem * item = m_Tree->currentItem();
//    if ( !item ) return;
//    if ( ( item == m_Tree->topLevelItem( 0 ) ) ||  ( m_FormList.count() < 2 ) )
//    {
//        QMessageBox::warning( this, qApp->applicationName(),
//                              tr( "You can not delete the root form." ) );
//        return;
//    }
//
//    // get selected treeitem
//    int id = item->data( LabelColumn, Qt::UserRole ).toInt();
//    //     qWarning() << id;
//    mfObject * mfo = m_FormList[id];
//
//    if ( mfo->isModified() )
//    {
//        if ( QMessageBox::warning( this, qApp->applicationName(),
//                                   tr( "The form %1 has been modified.\n"
//                                       "Do you really want to delete it ?" ).arg( mfo->label() ),
//                                   QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel )
//            return;
//    }
//
//    // delete all reference to itemwidget
//    if ( m_FormNavRecord )
//        m_FormNavOffset -= m_FormNavList.removeAll( item );
//
//    // clear all and redraw form
//    m_Tree->clear();
//    m_FormList.clear();
//
//    int i = 0;
//    for ( i = m_Stack->count(); i > -1; --i )
//    {
//        m_Stack->removeWidget( m_Stack->widget( i ) );
//        delete m_Stack->widget( i );
//    }
//
//    delete mfo;
//    createForm( m_RootObject );
//
//}

//void mfMainWidget::hideShowTree()
//{
//    m_Tree->setVisible( !m_Tree->isVisible() );
//}


//void mfMainWidget::translateTreeItem( QTreeWidgetItem * item )
//{
//    if ( !item ) return;
//    int id = item->data( LabelColumn, Qt::UserRole ).toInt();
//    item->setText( LabelColumn, m_FormList[id]->label() );
//    translateTreeItem( m_Tree->itemBelow( item ) );
//}

//void mfMainWidget::aboutToShowAddMenu()
//{
//    m_addMenu->clear();
//
//    QMenu *menu = m_addMenu->addMenu( tr( "a new form" ) );
//    QAction *action;
//
//    if ( m_Tree->currentItem() )
//    {
//        action = menu->addAction( tr( "as a child of \"%1\"" ).arg( m_Tree->currentItem()->text( 0 ) ) );
//        connect( action, SIGNAL( triggered() ), this, SLOT( addChildOnCurrentForm() ) );
//    }
//    action = menu->addAction( tr( "as a root" ) );
//    connect( action, SIGNAL( triggered() ), this, SLOT( addRootForm() ) );
//}

//void mfMainWidget::addRootForm()
//{
//}

//void mfMainWidget::gotoPrevious()
//{
//    if ( m_FormNavOffset > 0 )
//    {
//        m_FormNavOffset--;
//        m_FormNavRecord = false;
//        m_Tree->setCurrentItem( m_FormNavList[m_FormNavOffset] );
//        m_FormNavRecord = true;
//    }
//    nextAct->setEnabled( nextIsPossible() );
//    previousAct->setEnabled( previousIsPossible() );
//}

//void mfMainWidget::gotoNext()
//{
//    if ( m_FormNavOffset >= 0 && m_FormNavOffset < m_FormNavList.count() - 1 )
//    {
//        m_FormNavOffset++;
//        m_FormNavRecord = false;
//        m_Tree->setCurrentItem( m_FormNavList[m_FormNavOffset] );
//        m_FormNavRecord = true;
//    }
//    nextAct->setEnabled( nextIsPossible() );
//    previousAct->setEnabled( previousIsPossible() );
//}

//bool mfMainWidget::previousIsPossible() const
//{
//    return m_FormNavOffset > 0;
//}

//bool mfMainWidget::nextIsPossible() const
//{
//    return m_FormNavOffset < m_FormNavList.count() - 1;
//}
