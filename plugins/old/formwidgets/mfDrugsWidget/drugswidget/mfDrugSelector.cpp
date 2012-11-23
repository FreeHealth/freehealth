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
#include "mfDrugSelector.h"

#include <mfDrugsConstants.h>
#include <drugsdatabase/mfDrugsBase.h>

// include toolkit headers
#include <tkSettings.h>
#include <tkTheme.h>
#include <tkActionManager.h>

#include <QApplication>
#include <QSqlRecord>
#include <QHeaderView>


#ifndef DRUGS_INTERACTIONS_STANDALONE
#    include <mfCore.h>
#    include <mfSettings.h>
#endif

using namespace mfDrugsConstants;
Q_TK_USING_CONSTANTS

mfDrugSelector::mfDrugSelector( QWidget * parent ) :
        QWidget( parent ),
        m_DrugsModel(0), m_InnModel(0),
        m_SearchToolButton(0),
        m_DrugsHistoricButton(0),
        m_HistoryAct(0)
{
}

void mfDrugSelector::initialize()
{
    setupUi( this );

    createToolButtons();

    createDrugModelView();
    createINNModelView();
    createDrugsHistoryActions();
    createConnections();

    // select last search method (stored into qApp MFDRUGS_SETTING_SEARCHMETHOD)
    int m = tkSettings::instance()->value( MFDRUGS_SETTING_SEARCHMETHOD ).toInt();
    setSearchMethod(m);
    tkActionManager *am = tkActionManager::instance();
    QAction *a = 0;
    switch (m)
    {
        case mfDrugsConstants::SearchCommercial : a = am->command(mfDrugsConstants::A_SEARCH_COMMERCIAL)->action(); break;
        case mfDrugsConstants::SearchMolecules : a = am->command(mfDrugsConstants::A_SEARCH_MOLECULES)->action(); break;
        case mfDrugsConstants::SearchInn : a = am->command(mfDrugsConstants::A_SEARCH_INN)->action(); break;
    }
    if (a) {
        // TODO: Check the action in the menu/toolbutton... */
        a->trigger();
    }

    splitter->setStretchFactor( 0, 1 );
    splitter->setStretchFactor( 1, 3 );
    drugsView->setFocus();

    retranslateUi("");
}

void mfDrugSelector::setFont( const QFont & font )
{
    drugsView->setFont( font );
    InnView->setFont( font );
    searchLine->setFont( font );
}

void mfDrugSelector::createToolButtons()
{
    m_SearchToolButton = new QToolButton( searchLine );   // parent object will be redefined
    m_SearchToolButton->setPopupMode( QToolButton::InstantPopup );
    m_SearchToolButton->setToolTip( tr( "Select a search method using the icon on the left..." ) );
    m_SearchToolButton->setIcon( tkTheme::icon( ICONSEARCH ) );
    tkActionManager *am = tkActionManager::instance();
    tkCommand *cmd = am->command(mfDrugsConstants::A_SEARCH_COMMERCIAL);
    m_SearchToolButton->addAction(cmd->action());
    cmd = am->command(mfDrugsConstants::A_SEARCH_MOLECULES);
    m_SearchToolButton->addAction(cmd->action());
    cmd = am->command(mfDrugsConstants::A_SEARCH_INN);
    m_SearchToolButton->addAction(cmd->action());

    m_DrugsHistoricButton = new QToolButton( searchLine );
    m_DrugsHistoricButton->setPopupMode( QToolButton::InstantPopup );
    m_DrugsHistoricButton->setToolTip( tr( "Selected drugs historic." ) );
    m_DrugsHistoricButton->setIcon( tkTheme::icon( ICONEDIT) );

    // add buttons to search line
    searchLine->setLeftButton( m_SearchToolButton );
    searchLine->setRightButton( m_DrugsHistoricButton );
}

void mfDrugSelector::createDrugModelView()
{
    // insert SQL drugs model and table view
    m_DrugsModel = new QSqlTableModel( this, QSqlDatabase::database( DRUGS_DATABASE_NAME ) );
    m_DrugsModel->setTable( mfDrugsBase::instance()->table( Table_CIS ) );
    m_DrugsModel->setEditStrategy( QSqlTableModel::OnManualSubmit );
    QHashWhere where;
    where.insert( CIS_COMMERCIALISATION, "='O'" );
    m_DrugsModel->setFilter( mfDrugsBase::instance()->getWhereClause( Table_CIS, where ) );
    m_DrugsModel->select();
    // managing model fields
//    m_DrugsModel->removeColumns( CIS_ADMINISTRATION , 1 );
    m_DrugsModel->removeColumns( CIS_CODE_RPC , 1 );
    m_DrugsModel->removeColumns( CIS_COMMERCIALISATION , 1 );
    m_DrugsModel->removeColumns( CIS_AUTORISATION , 1 );
    m_DrugsModel->removeColumns( CIS_AMM , 1 );
    m_DrugsModel->removeColumns( CIS_FORME , 1 );
    // create the view
    drugsView->setModel( m_DrugsModel );
    //      drugsView->sortByColumn( 1 , Qt::AscendingOrder );  // NOT SUPPORTED BY WIN32 CROSS-COMPILATION !!!!
    drugsView->setColumnHidden( 0 , true );
    drugsView->verticalHeader()->hide();
    drugsView->horizontalHeader()->hide();
    drugsView->resizeColumnsToContents();
}

void mfDrugSelector::createINNModelView()
{    // create model and tableview for Iam Class / INNs
    m_InnModel = new QSqlTableModel( this, QSqlDatabase::database( DRUGS_DATABASE_NAME ) );
    m_InnModel->setTable( mfDrugsBase::instance()->table( Table_IAM_DENOMINATION ) );
    m_InnModel->setEditStrategy( QSqlTableModel::OnManualSubmit );
    QHashWhere where;
    where.insert( IAM_DENOMINATION_ID, ">999" );
    m_InnModel->setFilter( mfDrugsBase::instance()->getWhereClause( Table_IAM_DENOMINATION, where ) );
    m_InnModel->select();
    InnView->setModel( m_InnModel );
    InnView->setColumnHidden( 0 , true );
    InnView->resizeColumnsToContents();
    InnView->verticalHeader()->hide();
    InnView->horizontalHeader()->hide();
    InnView->hide();
}

void mfDrugSelector::createDrugsHistoryActions()
{
    if ( m_HistoryAct )
        delete m_HistoryAct;

    m_HistoryAct = new QActionGroup( this );
    foreach( QString s, tkSettings::instance()->value( MFDRUGS_SETTING_DRUGHISTORY ).toStringList() ) {
        QAction * a = new QAction( s.left( s.lastIndexOf( "," ) ).left( 70 ) + "..." , m_HistoryAct );
        a->setToolTip( s );
        m_HistoryAct->addAction ( a );
    }

    connect( m_HistoryAct, SIGNAL( triggered( QAction* ) ),
             this, SLOT( historyAct_triggered( QAction * ) ) );

    m_DrugsHistoricButton->addActions( m_HistoryAct->actions() );
}

void mfDrugSelector::createConnections()
{
    // connect line edit with model filter
    connect( searchLine, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateModel() ) );
    // connect search tool button actions
//    connect( searchCommercialAct, SIGNAL( triggered() ), this, SLOT( changeDrugsModelFilter() ) );
//    connect( searchMoleculeAct, SIGNAL( triggered() ), this, SLOT( changeDrugsModelFilter() ) );
//    connect( searchDCIAct, SIGNAL( triggered() ), this, SLOT( changeDrugsModelFilter() ) );
}

void mfDrugSelector::historyAct_triggered( QAction * action )
{
    // action tooltip == full drug name
    //    searchLine->setText( action->toolTip() );
    drugsView->setFocus();
    QHashWhere where;
    where.insert( CIS_COMMERCIALISATION, "='O'" );
    where.insert( CIS_DENOMINATION , QString( "= \"%1\"" ).arg( action->toolTip() ) );
    m_DrugsModel->setFilter( mfDrugsBase::instance()->getWhereClause( Table_CIS, where ));
}

//--------------------------------------------------------------------------------------------------------
//-------------------------------------- mfDrugSelector private slots -------------------------------------
//--------------------------------------------------------------------------------------------------------
void mfDrugSelector::retranslateUi( const QString & )
{
    if ( m_DrugsModel ) {
        m_DrugsModel->setHeaderData( 1, Qt::Horizontal, tr( "Short Name" ) );
        m_DrugsModel->setHeaderData( 2, Qt::Horizontal, tr( "Form" ) );
        m_DrugsModel->setHeaderData( 3, Qt::Horizontal, tr( "Administration" ) );
    }
}

void mfDrugSelector::setSearchMethod(int method)
{
    if ( method == mfDrugsConstants::SearchCommercial ) {
        m_filterModel = "";
        InnView->hide();
        m_SearchMethod = method;
        QHashWhere where;
        where.insert( CIS_COMMERCIALISATION, "='O'" );
        where.insert( CIS_DENOMINATION , "LIKE '__replaceit__%'" );
        m_filterModel = mfDrugsBase::instance()->getWhereClause( Table_CIS, where );
    }
    else if ( method == mfDrugsConstants::SearchMolecules ) {
        m_filterModel = "";
        InnView->hide();
        m_SearchMethod = method;
        // retreive all CIS for the searched molecule
        QHashWhere where;
        where.insert( COMPO_DENOMINATION , "LIKE '__replaceit__%'" );
        m_filterModel = mfDrugsBase::instance()->selectDistinct( Table_COMPO, COMPO_CIS , where );
//        m_filterModel += QString( "SELECT DISTINCT %1 FROM %2 WHERE %3" )
//                         .arg( mfDrugsTables::getField( mfDrugsTables::COMPO_CIS ) )
//                         .arg( mfDrugsTables::getTable( mfDrugsTables::Table_COMPO ) )
//                         .arg( mfDrugsTables::getWhereClause( map2 ) );
        // retreive drug name
        where.clear();
        where.insert( CIS_COMMERCIALISATION, "='O'" );
        where.insert( CIS_CIS , QString( "IN ( %1 ) " ).arg( m_filterModel ) );
        m_filterModel = mfDrugsBase::instance()->getWhereClause( Table_CIS, where );
    }
    else if ( method == mfDrugsConstants::SearchInn ) {
        m_filterModel = "";
        // show inn model and view
        InnView->show();
        // refresh inn select ?
        m_SearchMethod = method;
        // retreive all CIS for the searched inn
        QHashWhere where;
        where.insert( COMPO_CODE_SUBST , "IN ( __replaceit__ )" );
        m_filterModel += mfDrugsBase::instance()->selectDistinct( Table_COMPO, COMPO_CIS, where );
//        m_filterModel += QString( "SELECT DISTINCT %1 FROM %2 WHERE %3" )
//                         .arg( mfDrugsTables::getField( mfDrugsTables::COMPO_CIS ) )
//                         .arg( mfDrugsTables::getTable( mfDrugsTables::Table_COMPO ) )
//                         .arg( mfDrugsTables::getWhereClause( map2 ) );
        // retreive drug name
        where.clear();
        where.insert( CIS_COMMERCIALISATION, "='O'" );
        where.insert( CIS_CIS , QString( "IN ( %1 ) " ).arg( m_filterModel ) );
        m_filterModel = mfDrugsBase::instance()->getWhereClause( Table_CIS, where );
    }

    // store search method into qApp
    if (tkSettings::instance())
        tkSettings::instance()->setValue( MFDRUGS_SETTING_SEARCHMETHOD, m_SearchMethod );

    // update model
    updateModel();
}

void mfDrugSelector::updateModel()
{
    if ( searchLine->searchText().isEmpty() ) {
        m_DrugsModel->setFilter( "" );
        return;
    }
    QString tmp = m_filterModel;
    QString search = searchLine->searchText().replace("*", "%");
    if (m_SearchMethod != mfDrugsConstants::SearchInn)
        m_DrugsModel->setFilter( tmp.replace( "__replaceit__", search ) );
    else {
        // Search By INN
        // refresh inn view and model
        QHashWhere where;
        where.insert( IAM_DENOMINATION , QString( "LIKE '%1%' " ).arg( search ) );
        m_InnModel->setFilter(  mfDrugsBase::instance()->getWhereClause( Table_IAM_DENOMINATION, where ) );
        // retreive code_subst associated with searched text
        QList<int> codes = mfDrugsBase::instance()->getLinkedSubstCode( search );
        QString list = "";
        foreach( int i, codes )
            list += QString::number( i ) + ", " ;
        list.chop( 2 );
        m_DrugsModel->setFilter( tmp.replace( "__replaceit__", list ) );
    }
}

void mfDrugSelector::on_InnView_clicked( const QModelIndex & index )
{
    if (m_SearchMethod != mfDrugsConstants::SearchInn)
        return;

    if (!index.isValid())
        return;

    QString inn = index.data().toString();
    // retreive code_subst associated with searched text
    QString tmp = m_filterModel;
    QList<int> codes = mfDrugsBase::instance()->getLinkedSubstCode( inn );
    QString list = "";
    foreach( int i, codes )
        list += QString::number( i ) + ", " ;
    list.chop( 2 );
    m_DrugsModel->setFilter( tmp.replace( "__replaceit__", list ) );
}

void mfDrugSelector::on_drugsView_doubleClicked( const QModelIndex & index )
{
    // retreive CIS of drug and emit signal
    if ( !index.isValid() )
        return;
    QSqlRecord rec = m_DrugsModel->record( index.row() );
    int selectedCIS = rec.value( 0 ).toInt();

    // store drug into history
    QStringList hist = tkSettings::instance()->value( MFDRUGS_SETTING_DRUGHISTORY ).toStringList();
    hist.removeAll( index.data().toString() );
    if (hist.count() && ( hist.count() == tkSettings::instance()->value( MFDRUGS_SETTING_HISTORYSIZE ).toInt() ) )
        hist.removeFirst();
    hist << index.data().toString();
    tkSettings::instance()->setValue( MFDRUGS_SETTING_DRUGHISTORY, hist );

    // refresh drug history
    createDrugsHistoryActions();

    emit drugSelected( selectedCIS );
    emit drugSelected( index );
}
