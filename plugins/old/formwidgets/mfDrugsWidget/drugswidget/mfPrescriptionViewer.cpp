/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                     **
 *   eric.maeker@gmail.com                                                   *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "mfPrescriptionViewer.h"

#include <mfDrugsConstants.h>
#include <drugsmodel/mfDrugs.h>
#include <drugsmodel/mfDrugsModel.h>
#include <drugswidget/mfDrugInfo.h>
#include <drugswidget/mfInteractionDialog.h>
#include <dosagedialog/mfDosageCreatorDialog.h>
#include <dosagedialog/mfDosageDialog.h>
#include <mfDrugsManager.h>

#include <tkTheme.h>
#include <tkLog.h>
#include <tkActionManager.h>
#include <tkContextManager.h>

#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>

using namespace mfDrugsConstants;
Q_TK_USING_CONSTANTS

/** \brief Constructor. You must call initialize() after instanciation */
mfPrescriptionViewer::mfPrescriptionViewer(QWidget *parent) :
        QWidget(parent),
        m_ToolBar(0)
{
    setObjectName("mfPrescriptionViewer");
    setupUi(this);
}

/** \brief Creates actions, popup menus and toolbars */
void mfPrescriptionViewer::initialize()
{
    createActionsAndToolbar();
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
    tkActionManager *am = tkActionManager::instance();
    tkCommand *cmd = 0;
    // populate toolbar
    m_ToolBar = new QToolBar(this);
    m_ToolBar->setIconSize(QSize(16, 16));

    QStringList actionsToAdd;
    actionsToAdd
            << tkConstants::A_FILE_OPEN
            << tkConstants::A_FILE_SAVE
            << tkConstants::A_FILE_SAVEAS
            << mfDrugsConstants::A_PRINT_PRESCRIPTION;

    foreach(const QString &s, actionsToAdd) {
        cmd = am->command(s);
        m_ToolBar->addAction(cmd->action());
    }

    actionsToAdd.clear();
    actionsToAdd
            << tkConstants::A_LIST_CLEAR
            << tkConstants::A_LIST_REMOVE
            << tkConstants::A_LIST_MOVEDOWN
            << tkConstants::A_LIST_MOVEUP
            << tkConstants::A_LIST_SORT
            ;
    m_ToolBar->addSeparator();
    foreach(const QString &s, actionsToAdd) {
        cmd = am->command(s);
        m_ToolBar->addAction(cmd->action());
    }
    m_ToolBar->addSeparator();

    actionsToAdd.clear();
    actionsToAdd
            << mfDrugsConstants::A_TOOGLE_TESTINGDRUGS
            << tkConstants::A_VIEW_INTERACTIONS;

    foreach(const QString &s, actionsToAdd) {
        cmd = am->command(s);
        m_ToolBar->addAction(cmd->action());
        m_ToolBar->addSeparator();
    }
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
//    checkMovableItem( m->index(row-1,0) );
}

/** \brief Move the selected drug down. */
void mfPrescriptionViewer::moveDown()
{
    Q_ASSERT_X( static_cast<mfDrugsModel*>(listView->model()), "mfPrescriptionViewer", "model is not a mfDrugsModel*");
    mfDrugsModel *m = static_cast<mfDrugsModel*>(listView->model());
    int row = listView->currentIndex().row();
    m->moveDown(listView->currentIndex());
    listView->setCurrentIndex( m->index(row+1,0) );
//     checkMovableItem( m->index(row+1,0) );
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
    int CIS = DRUGMODEL->index( item.row(), Drug::CIS ).data().toInt();
    mfDosageDialog dlg(this);
    dlg.changeRow(CIS,item.row());
    dlg.exec();
//    listView->repaint();
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
//        retranslateActions();
        break;
    default:
        break;
    }
}
