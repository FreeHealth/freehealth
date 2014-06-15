/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2014 by Eric MAEKER, MD                                     **
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
 *  Main Developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "tkUserManager.h"
#include "tkUserManager_p.h"

// include toolkit headers
#include <tkLog.h>
#include <tkTheme.h>
#include <QButtonLineEdit.h>

// include usertoolkit headers
#include <tkUser.h>
#include <tkUserViewer.h>
#include <tkUserConstants.h>
#include <tkUserModel.h>
#include <tkUserWizard.h>

// include Qt headers
#include <QUuid>
#include <QModelIndex>
#include <QTableView>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QSplitter>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QToolButton>
#include <QLabel>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QGroupBox>

Q_TKUSER_USING_CONSTANTS
Q_TK_USING_CONSTANTS

/**
  \brief Main user interface for User Manager.
  User Model must have been instanciated BEFORE this interface, and a current user must have been setted
  \sa tkUserModel, tkUserModel::hasCurrentUser()
  \todo Search user by city, search by name & surname,
  \ingroup widget_usertoolkit usertoolkit usermanager
*/
tkUserManager::tkUserManager( QWidget * parent )
    : QMainWindow( parent )
{
    Q_ASSERT_X( tkUserModel::instance()->hasCurrentUser(), "tkUserManager", "NO CURRENT USER" );
    if ( ! tkUserModel::instance()->hasCurrentUser() == QVariant() )
        return;
    setAttribute( Qt::WA_DeleteOnClose );
    d = new tkUserManagerPrivate(this);
    setUnifiedTitleAndToolBarOnMac(true);
}

/** \brief Close the usermanager. Check if modifications have to be saved and ask user. */
void tkUserManager::closeEvent( QCloseEvent *event )
{
    if ( tkUserModel::instance()->hasUserToSave() ) {
        int ret = tkGlobal::withButtonsMessageBox( tr("You've modified the users' list."), tr("Do you want to save your changes ?"), "",
                                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                         QMessageBox::Save, windowTitle() );
        if ( ret == QMessageBox::Discard )
            event->accept();
        else if ( ret == QMessageBox::Cancel )
            event->ignore();
        else if ( tkUserModel::instance()->submitAll() ) {
            QMessageBox::information( this, windowTitle(), tr( "Changes have been correctly saved." ) );
            event->accept();
        }
        else {
            QMessageBox::information( this, windowTitle(), tr( "Changes can not be correctly saved." ) );
            event->ignore();
        }
    }
    else
        event->accept();
}

/** \brief Destructor */
tkUserManager::~tkUserManager()
{
    // Qt will delete Private part
}

/**
  \brief UserManager Main Ui interface.
  \internal
  \ingroup widget_usertoolkit usertoolkit usermanager
*/
tkUserManagerPrivate::tkUserManagerPrivate( QMainWindow * parent )
    : QObject( parent ),
    m_PermanentMemoryUsage(0),
    m_PermanentUserName(0),
    m_PermanentWidget(0),
    groupBoxMemoryUsage(0)
{
    m_Parent = parent;
    m_SearchBy = User::UserName;
    setupUi( parent );

    toolBar->addActions( menuUser_manager->actions() );
    // insert central widget
    QWidget * central = new QWidget( parent );
    QVBoxLayout * vb = new QVBoxLayout( central );
    //    vb->setMargin( 5 );
    QSplitter * split = new QSplitter( central );

    // Prepare left widget
    QWidget * left = new QWidget( central );
    QVBoxLayout * vbleft = new QVBoxLayout( left );
    vbleft->setSpacing( 10 );
    vbleft->setMargin( 0 );
    m_UserTableView = new QTableView( left );
    m_UserTableView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    tkUserModel * model = tkUserModel::instance();
    m_UserTableView->setModel( model );
    m_UserTableView->horizontalHeader()->hide();
    m_UserTableView->verticalHeader()->hide();
    m_UserTableView->hideColumn( USER_ID );
    m_UserTableView->hideColumn( USER_UUID );
    m_UserTableView->hideColumn( USER_LOGIN );
    m_UserTableView->hideColumn( USER_PASSWORD );
    m_UserTableView->hideColumn( USER_LANGUAGE );
    m_UserTableView->hideColumn( USER_MAIL );
    m_UserTableView->hideColumn( USER_VALIDITY );
    m_UserTableView->hideColumn( USER_SECONDNAME );
    m_UserTableView->hideColumn( USER_LASTLOG );
    m_UserTableView->hideColumn( USER_LOCKER );
    m_UserTableView->resizeColumnsToContents();
    m_UserTableView->setSelectionMode( QAbstractItemView::SingleSelection );
    m_UserTableView->setSelectionBehavior( QAbstractItemView::SelectRows );
    // prepare Search Line Edit
    m_SearchEdit = new QButtonLineEdit( left );
    m_SearchToolBut = new QToolButton( m_SearchEdit );
    m_SearchEdit->setLeftButton( m_SearchToolBut );
    searchByNameAct = new QAction( m_SearchToolBut );
    searchBySurnameAct = new QAction( m_SearchToolBut );
    searchByNameAndSurnameAct = new QAction( m_SearchToolBut );
    searchByCityAct = new QAction( m_SearchToolBut );
    m_SearchToolBut->addAction( searchByNameAct );
    m_SearchToolBut->addAction( searchBySurnameAct );
    //    m_SearchToolBut->addAction( searchByNameAndSurnameAct );
    //    m_SearchToolBut->addAction( searchByCityAct );
    m_SearchToolBut->setPopupMode( QToolButton::InstantPopup );
    // group memory usage
    groupBoxMemoryUsage = new QGroupBox(left);
    QVBoxLayout *groupLayout = new QVBoxLayout(groupBoxMemoryUsage);
    m_PermanentMemoryUsage = new QLabel( m_PermanentWidget );
    groupLayout->addWidget( m_PermanentMemoryUsage );
    retranslate();

    // populate left widget layout
    vbleft->addWidget( m_SearchEdit );
    vbleft->addWidget( m_UserTableView );
    vbleft->addWidget( groupBoxMemoryUsage );
    selectUserTableView( tkUserModel::instance()->currentUserIndex().row() );

    // create center user viewer
    m_UserViewer = new tkUserViewer( central );
    split->addWidget( left );
    split->addWidget( m_UserViewer );
    split->setStretchFactor( 0, 1 );
    split->setStretchFactor( 1, 3 );
    vb->addWidget( split );

    analyseCurrentUserRights();

    //    userTableView->hide();
    parent->setCentralWidget( central );
    saveAct->setShortcut( QKeySequence::Save );
    createNewUserAct->setShortcut( QKeySequence::New );

    // connect slots
    connect( saveAct, SIGNAL(triggered()), this, SLOT(on_saveAct_triggered()) );
    connect( createNewUserAct, SIGNAL(triggered()), this, SLOT(on_createNewUserAct_triggered()) );
    connect( clearModificationsAct, SIGNAL(triggered()), this, SLOT(on_clearModificationsAct_triggered()) );
    connect( deleteUserAct,  SIGNAL(triggered()), this, SLOT(on_deleteUserAct_triggered()) );
    connect( quitUserManagerAct,  SIGNAL(triggered()), m_Parent, SLOT(close()) );
    connect( m_UserTableView, SIGNAL( activated( const QModelIndex & ) ),
             this, SLOT( on_userTableView_activated( const QModelIndex & ) ) );
    // connections for search line edit
    connect( m_SearchEdit, SIGNAL(textChanged(const QString &)), this, SLOT( on_m_SearchEdit_textchanged() ) );
    connect( m_SearchToolBut, SIGNAL(triggered(QAction*)), this, SLOT(on_m_SearchToolButton_triggered(QAction*)) );

    updateStatusBar();
    connect( tkUserModel::instance(), SIGNAL(memoryUsageChanged()), this, SLOT(updateStatusBar()) );

    // manage theme / icons
    parent->setWindowIcon( tkTheme::icon( ICONUSERMANAGER ) );
    saveAct->setIcon( tkTheme::icon( ICONSAVE, tkTheme::MediumIcon ) );
    createNewUserAct->setIcon( tkTheme::icon( ICONNEWUSER, tkTheme::MediumIcon ) );
    modifyUserAct->setIcon( tkTheme::icon( ICONEDITUSER, tkTheme::MediumIcon ) );
    clearModificationsAct->setIcon( tkTheme::icon( ICONCLEARUSER, tkTheme::MediumIcon ) );
    deleteUserAct->setIcon( tkTheme::icon( ICONDELETEUSER, tkTheme::MediumIcon ) );
    quitUserManagerAct->setIcon( tkTheme::icon( ICONEXIT, tkTheme::MediumIcon ) );
    m_SearchToolBut->setIcon( tkTheme::icon( ICONSEARCH ) );
    searchByNameAct->setIcon( tkTheme::icon( ICONSEARCH ) );
    searchBySurnameAct->setIcon( tkTheme::icon( ICONSEARCH ) );
    searchByNameAndSurnameAct->setIcon( tkTheme::icon( ICONSEARCH ) );
    searchByCityAct->setIcon( tkTheme::icon( ICONSEARCH ) );

    // TODO active userTableView on currentUser
}

void tkUserManagerPrivate::analyseCurrentUserRights()
{
    // retreive user manager rights from model
    tkUserModel *m = tkUserModel::instance();
    User::UserRights r ( m->data( m->index( m->currentUserIndex().row(), User::UserManagerRights ) ).toInt() );
    // translate to bools
    m_CanModify = ( r & User::WriteAll );
    m_CanCreate = ( r & User::Create );
    m_CanViewAllUsers = ( r & User::ReadAll );
    m_CanDelete = ( r & User::Delete );

    // manage ui <-> rights
    createNewUserAct->setEnabled( m_CanCreate );
    modifyUserAct->setEnabled( m_CanModify );
    deleteUserAct->setEnabled( m_CanDelete );

    // manage specific creation widgets
    m_UserTableView->setVisible( m_CanViewAllUsers );
    m_SearchEdit->setVisible( m_CanViewAllUsers );
}

void tkUserManagerPrivate::updateStatusBar()
{
    tkUserModel *m = tkUserModel::instance();
    if ( ! m_PermanentWidget ) {
        m_PermanentWidget = new QWidget(m_Parent);
        QHBoxLayout * l = new QHBoxLayout( m_PermanentWidget );
        l->setMargin(0);
        if ( ! m_PermanentUserName )
            m_PermanentUserName = new QLabel( m_PermanentWidget );
        l->addWidget( m_PermanentUserName );
    }
    m_PermanentMemoryUsage->setText( tr( "Database usage : %1,\nMemory usage : %2" )
                                     .arg( m->rowCount())
                                     .arg(m->numberOfUsersInMemory()) );
    m_PermanentUserName->setText( m->index( m->currentUserIndex().row(), User::UserName ).data().toString() );
    m_Parent->statusBar()->addPermanentWidget(m_PermanentWidget);
}

void tkUserManagerPrivate::on_m_SearchToolButton_triggered( QAction * act )
{
    if ( act == searchByNameAct )
        m_SearchBy= User::UserName;
    else if (act == searchBySurnameAct)
        m_SearchBy= User::UserSurname;
    else if (act == searchByNameAndSurnameAct)
        m_SearchBy= -1;
    else if (act == searchByCityAct)
        m_SearchBy= User::UserCity;
}

void tkUserManagerPrivate::on_m_SearchEdit_textchanged()
{
    // TODO : Manage error when user select an action in the toolbutton
    // TODO : where can only be calculated by model
    QHash<int, QString> where;
    where.insert( m_SearchBy, QString( "LIKE '%1%'" ).arg( m_SearchEdit->searchText() ) );
    tkUserModel::instance()->setFilter( where );
}

void tkUserManagerPrivate::on_createNewUserAct_triggered()
{
    int createdRow = m_UserTableView->model()->rowCount();
    if ( ! m_UserTableView->model()->insertRows( createdRow, 1 ) ) {
        tkLog::addError( this, "Cannot create new user : can not add row to model");
        return;
    }
    QModelIndex index = m_UserTableView->model()->index( createdRow, USER_NAME );
//    m_UserTableView->model()->setData( index, tr( "New User" ) );
    tkUserWizard wiz(m_Parent);
    wiz.setModelRow( createdRow );
    int r = wiz.exec();
    if ( r == QDialog::Rejected ) {
        if ( ! m_UserTableView->model()->removeRows( createdRow, 1 ) ) {
            tkLog::addError( this, "Cannot delete new user : can not delete row to model");
            return;
        } else
            m_Parent->statusBar()->showMessage( tr( "No user created" ), 2000 );
    } else {
        m_UserTableView->selectRow( createdRow );
        on_userTableView_activated( index );
        m_Parent->statusBar()->showMessage( tr( "User created" ), 2000 );
    }
    qApp->setActiveWindow( m_Parent );
    m_Parent->activateWindow();
}

void tkUserManagerPrivate::on_clearModificationsAct_triggered()
{
    if (tkUserModel::instance()->revertAll())
        m_Parent->statusBar()->showMessage( tr( "Modifications cleared" ), 2000 );
    else
        m_Parent->statusBar()->showMessage( tr( "Can not clear modifications" ), 2000 );
}

void tkUserManagerPrivate::on_saveAct_triggered()
{
    if ( ( ! m_CanModify ) || ( ! m_CanCreate ) )
        return;
    // save changes to database
    if ( tkUserModel::instance()->submitAll() )
        m_Parent->statusBar()->showMessage( tr( "User saved" ), 2000 );
}

void tkUserManagerPrivate::on_deleteUserAct_triggered()
{
    if ( tkUserModel::instance()->removeRow( m_UserTableView->currentIndex().row() ) )
        m_Parent->statusBar()->showMessage( tr( "User deleted" ), 2000 );
    else
        m_Parent->statusBar()->showMessage( tr( "User can not be deleted" ), 2000 );
}

void tkUserManagerPrivate::on_userTableView_activated( const QModelIndex & index )
{
    m_UserViewer->changeUserTo( index.row() );
}

void  tkUserManagerPrivate::selectUserTableView( int row )
{
    m_UserTableView->selectRow( row );
}

void tkUserManagerPrivate::changeEvent(QEvent *e)
{
    if ( (e->type() == QEvent::LanguageChange) ) {
        retranslateUi( m_Parent );
        m_Parent->setWindowTitle( tr( "User Manager" ) + " - " + qApp->applicationName() );
    }
    retranslate();
}

void tkUserManagerPrivate::retranslate()
{
    searchByNameAct->setText( tr( "Search user by name" ) );
    searchBySurnameAct->setText( tr( "Search user by surname" ) );
    searchByNameAndSurnameAct->setText( tr( "Search user by name and surname" ) );
    searchByCityAct->setText( tr( "Search user by city" ) );
    searchByNameAct->setToolTip( tr( "Search user by name" ) );
    searchBySurnameAct->setToolTip( tr( "Search user by surname" ) );
    searchByNameAndSurnameAct->setToolTip( tr( "Search user by name and surname" ) );
    searchByCityAct->setToolTip( tr( "Search user by city" ) );
    m_SearchToolBut->setToolTip( tr( "Search a user" ) );
    groupBoxMemoryUsage->setTitle( tr("Memory usage") );
    m_Parent->setWindowTitle( tr( "User Manager" ) + " - " + qApp->applicationName() );
    updateStatusBar();
}
