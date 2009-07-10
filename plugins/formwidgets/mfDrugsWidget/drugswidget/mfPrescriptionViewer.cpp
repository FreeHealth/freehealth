/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "mfPrescriptionViewer.h"

#include <mfDrugsConstants.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugswidget/mfDrugInfo.h>
#include <drugswidget/mfDosageDialog.h>
#include <drugswidget/mfInteractionDialog.h>
#include <tkTheme.h>
#include <tkLog.h>
#include <tkActionManager.h>

#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>

using namespace mfDrugsConstants;
Q_TK_USING_CONSTANTS

namespace mfPrescriptionViewerPrivateConstants {
    const char * const  A_VIEW_INTERACTIONS     = "actionViewInteractions";
    const char * const  VIEW_INTERACTIONS_TEXT  = QT_TRANSLATE_NOOP("mfPrescriptionViewer", "View synthetic interactions");
}

using namespace mfPrescriptionViewerPrivateConstants;

/** \brief Constructor. You must call initialize() after instanciation */
mfPrescriptionViewer::mfPrescriptionViewer(QWidget *parent) :
        QWidget(parent), saveAct(0), clearAct(0), printAct(0), removeAct(0),
        moveUpAct(0), moveDownAct(0), sortAct(0), viewAct(0),
        m_ToolBar(0)
{
    setObjectName("mfPrescriptionViewer");
    setupUi(this);
}

/** \brief Creates actions, popup menus and toolbars */
void mfPrescriptionViewer::initialize()
{
    createActionsAndToolbar();
    createConnections();
    verticalLayout->insertWidget( 0, m_ToolBar );
    listView->setObjectName("mfPrescriptionListView");
    setListViewPadding( 5 );
//    listView->setMovement( QListView::Snap );
}

/** \brief Defines the model to use. */
void mfPrescriptionViewer::setModel( mfDrugsModel *model )
{
    Q_ASSERT_X( model, "mfPrescriptionViewer::setModel", "Model must be correctly setted");
    listView->setModel( model );
}

/** \brief Defines the model's column to use. */
void mfPrescriptionViewer::setModelColumn( const int col )
{
    Q_ASSERT_X( static_cast<mfDrugsModel*>(listView->model()), "mfPrescriptionViewer::setModelColumn", "model is not a mfDrugsModel*");
    listView->setModelColumn(col);
}

void mfPrescriptionViewer::setListViewPadding( const int pad )
{
//    listView->setSpacing(pad);
    listView->setStyleSheet( QString("QListView#mfPrescriptionListView:item { padding: %1px; }").arg(pad) );
}

/** \brief Creates actions and toolbar */
void mfPrescriptionViewer::createActionsAndToolbar()
{
    m_ToolBar = new QToolBar( this );
    m_ToolBar->setIconSize( QSize( 16, 16 ) );

    tkActionManager *am = tkActionManager::instance();
    saveAct = am->action(A_FILE_SAVE);
    m_ToolBar->addAction( saveAct );

    printAct = am->action(A_FILE_PRINT);
    m_ToolBar->addAction( printAct );

    removeAct = new QAction( this );
    removeAct->setIcon( tkTheme::icon( ICONREMOVE ) );
    removeAct->setShortcut( QKeySequence::Delete );
    m_ToolBar->addAction( removeAct );

    clearAct = new QAction( this );
    clearAct->setIcon( tkTheme::icon( ICONCLEAR ) );
    clearAct->setShortcut( tr( "Ctrl+W" ) );
    m_ToolBar->addAction( clearAct );

    moveUpAct = new QAction( this );
    moveUpAct->setIcon( tkTheme::icon( ICONMOVEUP ) );
    moveUpAct->setShortcut( tr( "Ctrl+W" ) );
    moveUpAct->setEnabled(false);
    m_ToolBar->addAction( moveUpAct );

    moveDownAct = new QAction( this );
    moveDownAct->setIcon( tkTheme::icon( ICONMOVEDOWN ) );
    moveDownAct->setShortcut( tr( "Ctrl+W" ) );
    moveDownAct->setEnabled(false);
    m_ToolBar->addAction( moveDownAct );

    sortAct = new QAction( this );
    sortAct->setIcon( tkTheme::icon( ICONSORT ) );
    sortAct->setShortcut( tr( "Ctrl+W" ) );
    m_ToolBar->addAction( sortAct );

    viewAct = new QAction( this );
    viewAct->setIcon( tkTheme::icon( ICONEYES ) );
    viewAct->setShortcut( tr( "Ctrl+W" ) );
    viewAct->setToolTip( tr(VIEW_INTERACTIONS_TEXT) );
    m_ToolBar->addAction( viewAct );

    retranslateActions();
}

/** \brief creates all connections */
void mfPrescriptionViewer::createConnections()
{
//    connect( printAct, SIGNAL(triggered()), this, SIGNAL(printTriggered()));
//    connect( saveAct, SIGNAL(triggered()), this, SLOT(save()));
//    connect( saveAct, SIGNAL(triggered()), this, SIGNAL(saveTriggered()));
    connect( clearAct, SIGNAL(triggered()), this, SLOT(clearTriggered()));
    connect( removeAct, SIGNAL(triggered()), this, SLOT(removeTriggered()));
    connect( moveUpAct, SIGNAL( triggered() ), this, SLOT( moveUp() ) );
    connect( moveDownAct, SIGNAL( triggered() ), this, SLOT( moveDown() ) );
    connect( sortAct, SIGNAL( triggered() ), this, SLOT( sortDrugs() ) );
    connect( viewAct, SIGNAL( triggered() ), this, SLOT( viewInteractions() ) );
    connect( listView, SIGNAL(clicked(const QModelIndex &)),
             this, SLOT(checkMovableItem(const QModelIndex &)));
//    connect( listView, SIGNAL(activated(const QModelIndex &)), this, SLOT(showDosageDialog(const QModelIndex&)) );
}

/** \brief */
void mfPrescriptionViewer::checkMovableItem( const QModelIndex & current)//, const QModelIndex & )
{
    Q_ASSERT_X( static_cast<mfDrugsModel*>(listView->model()), "mfPrescriptionViewer", "model is not a mfDrugsModel*");
    mfDrugsModel *m = static_cast<mfDrugsModel*>(listView->model());
    // test moveUp
    if (current.row() >= 1)
        moveUpAct->setEnabled(true);
    else
        moveUpAct->setEnabled(false);
    // test moveDown
    if (current.row() < (m->rowCount()-1))
        moveDownAct->setEnabled(true);
    else
        moveDownAct->setEnabled(false);
}

/** \brief Clears the prescription */
void mfPrescriptionViewer::clearTriggered()
{
    Q_ASSERT_X( static_cast<mfDrugsModel*>(listView->model()), "mfPrescriptionViewer", "model is not a mfDrugsModel*");
    static_cast<mfDrugsModel*>(listView->model())->clearDrugsList();
}

/** \brief Remove the selected drug from the prescription.*/
void mfPrescriptionViewer::removeTriggered()
{
    Q_ASSERT_X( static_cast<mfDrugsModel*>(listView->model()), "mfPrescriptionViewer", "model is not a mfDrugsModel*");
    if (!listView->selectionModel()->hasSelection() )
        return;
    const QModelIndexList &list = listView->selectionModel()->selectedRows(0);
    foreach( const QModelIndex &index, list ) {
        static_cast<mfDrugsModel*>(listView->model())->removeRows( index.row(), 1 );
    }
}

/** \brief Moves the selected drug up. */
void mfPrescriptionViewer::moveUp()
{
    Q_ASSERT_X( static_cast<mfDrugsModel*>(listView->model()), "mfPrescriptionViewer", "model is not a mfDrugsModel*");
    mfDrugsModel *m = static_cast<mfDrugsModel*>(listView->model());
    int row = listView->currentIndex().row();
    m->moveUp(listView->currentIndex());
    listView->setCurrentIndex( m->index(row-1,0) );
    checkMovableItem( m->index(row-1,0) );
}

/** \brief Move the selected drug down. */
void mfPrescriptionViewer::moveDown()
{
    Q_ASSERT_X( static_cast<mfDrugsModel*>(listView->model()), "mfPrescriptionViewer", "model is not a mfDrugsModel*");
    mfDrugsModel *m = static_cast<mfDrugsModel*>(listView->model());
    int row = listView->currentIndex().row();
    m->moveDown(listView->currentIndex());
    listView->setCurrentIndex( m->index(row+1,0) );
    checkMovableItem( m->index(row+1,0) );
}

/** \brief Sorts the drugs of the prescription. \sa mfDrugsModel::sort() */
void mfPrescriptionViewer::sortDrugs()
{
    listView->model()->sort(0);
}

/** \brief Opens the drug information dialog. \sa mfDrugInfo */
void mfPrescriptionViewer::showDrugInfo(const QModelIndex &item)
{
    mfDrugInfo di(item.row(), this);
    di.exec();
}

/** \brief Opens the mfDosageDialog for the selected drug. */
void mfPrescriptionViewer::showDosageDialog(const QModelIndex &item)
{
//    mfDosageDialog dialog(this, item.row(), 0 );
    mfDosageDialog dialog(this);
    dialog.exec();
}

/** \brief Opens the mfInteractionDialog. */
void mfPrescriptionViewer::viewInteractions()
{
     mfInteractionDialog dlg(this);
     dlg.exec();
}

/** \brief Returns the listView in use for the prescription. */
QListView *mfPrescriptionViewer::listview()
{
    return listView;
}

/** \brief Used for translations. \internal */
void mfPrescriptionViewer::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        retranslateActions();
        break;
    default:
        break;
    }
}

/** \brief Used for translations. \internal */
void mfPrescriptionViewer::retranslateActions()
{
    removeAct->setToolTip( tr("Remove this drug") );
    clearAct->setToolTip( tr("Clear this prescription") );
    sortAct->setToolTip( tr("Sort this prescription") );
}
